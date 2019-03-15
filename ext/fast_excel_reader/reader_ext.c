#include "ruby.h"
#include <stdio.h>
#include <string.h>
#include <xlsxio_read.h>

// days between 1970-jan-01 and 1900-jan-01
static double XLSX_DATE_DAY = 86400.0;
static double XLSX_DATE_EPOCH_DIFF = 25569.0;

void xlsxioread_finalizer(xlsxioreader xlsxioread) {
  printf("- xlsxioread_close\n");
  if (xlsxioread) {
    xlsxioread_close(xlsxioread);
  }
  printf("- xlsxioread_close done\n");
}

VALUE rb_reader_open(VALUE klass, VALUE filename) {
  char *filename_str = StringValueCStr(filename);

  xlsxioreader xlsxioread;

  if ((xlsxioread = xlsxioread_open(filename_str)) == NULL) {
    rb_raise(rb_eArgError, "Error opening .xlsx file");
    return Qnil;
  }

  VALUE result = Data_Wrap_Struct(klass, NULL, xlsxioread_finalizer, xlsxioread);

  return result;
}

VALUE rb_sheet_names(VALUE self) {
  xlsxio_read_struct * xlsxioread;
  Data_Get_Struct(self, struct xlsxio_read_struct, xlsxioread);

  VALUE rb_sheet_names = rb_ary_new();

  xlsxioreadersheetlist sheetlist;
  const XLSXIOCHAR* sheetname;

  if ((sheetlist = xlsxioread_sheetlist_open(xlsxioread)) != NULL) {
    while ((sheetname = xlsxioread_sheetlist_next(sheetlist)) != NULL) {
      rb_ary_push(rb_sheet_names, rb_str_new2(sheetname));
    }
    xlsxioread_sheetlist_close(sheetlist);
  }

  return rb_sheet_names;
}

VALUE rb_yield_values_1(VALUE arg) {
  return rb_yield_values(1, arg);
}

VALUE rb_each_row(int argc, VALUE* argv, VALUE self) {
  xlsxio_read_struct * xlsxioread;
  Data_Get_Struct(self, struct xlsxio_read_struct, xlsxioread);
  VALUE cTime = rb_const_get(rb_cObject, rb_intern("Time"));

  VALUE sheet_name = Qnil, tz_offset = Qnil, opts, kwargs[2];
  rb_scan_args(argc, argv, ":", &opts);

  if (!NIL_P(opts)) {
    static ID kwargs_ids[2];

    kwargs_ids[0] = rb_intern_const("sheet");
    kwargs_ids[1] = rb_intern_const("tz_offset");

    rb_get_kwargs(opts, kwargs_ids, 0, 2, kwargs);
    sheet_name = kwargs[0];
    tz_offset = kwargs[1];
  }

  char *sheet_name_str = NULL;
  if (!NIL_P(sheet_name)) {
    sheet_name_str = StringValueCStr(sheet_name);
  }

  if (NIL_P(tz_offset) || tz_offset == Qundef) {
    VALUE timeObject = rb_funcall(cTime, rb_intern("at"), 1, DBL2NUM(0));
    tz_offset = rb_funcall(timeObject, rb_intern("utc_offset"), 0);
  }

  //printf("sheet name: %s tz_offset: %f \n", sheet_name_str, NUM2DBL(tz_offset));

  xlsxioreadersheet sheet = xlsxioread_sheet_open(xlsxioread, sheet_name_str, XLSXIOREAD_SKIP_EMPTY_ROWS);

  if (sheet == NULL) {
    if (sheet_name_str) {
      rb_raise(rb_eArgError, "Can not find sheet %"PRIsVALUE" in file", sheet_name);
    } else {
      rb_raise(rb_eArgError, "Can not find any sheet in xlsx file");
    }
    return T_FALSE;
  }

  xlsxioread_cell value;

  double tz_offset_dbl = NUM2DBL(tz_offset);
  double float_val, timestamp;
  int yieldException = 0;

  while (xlsxioread_sheet_next_row(sheet)) {
    VALUE rb_cells = rb_ary_new();

    // see cell_type_enum definition in xlsxio_read.h

    while ((value = xlsxioread_sheet_next_cell_struct(sheet)) != NULL) {
      switch (value->cell_type) {
        case cell_type_none: case cell_type_string:
          rb_ary_push(rb_cells, rb_str_new2(value->data));
          break;
        case cell_type_boolean:
          rb_ary_push(rb_cells, strcmp(value->data, "1") == 0 ? T_TRUE : T_FALSE);
          break;
        case cell_type_value:
          float_val = atof(value->data);
          rb_ary_push(rb_cells, DBL2NUM(float_val));
          break;
        case cell_type_date:
          timestamp = (atof(value->data) - XLSX_DATE_EPOCH_DIFF) * XLSX_DATE_DAY - tz_offset_dbl;
          VALUE timeObject = rb_funcall(cTime, rb_intern("at"), 1, DBL2NUM(timestamp));
          rb_ary_push(rb_cells, timeObject);
          break;
      }

      //printf("free 1\n");
      //if (value->data) {
      //  free(value->data);
      //  value->data = NULL;
      //}
      //printf("free 2\n");
      //if (value->number_fmt) {
      //  printf("free 2.1\n");
      //  free(value->number_fmt);
      //  value->number_fmt = NULL;
      //}
      //printf("free 3\n");
      free(value);
    }

    if (rb_block_given_p()) {
      rb_protect(rb_yield_values_1, rb_cells, &yieldException);
      // exit if error happen or "break" was called
      if (yieldException) {
        xlsxioread_sheet_close(sheet);
        rb_jump_tag(yieldException);
      }
    }
  }

  xlsxioread_sheet_close(sheet);

  return T_TRUE;
}

VALUE rb_close(VALUE self) {

  xlsxio_read_struct * xlsxioread;
  Data_Get_Struct(self, struct xlsxio_read_struct, xlsxioread);
  xlsxioread_finalizer(xlsxioread);

  return T_TRUE;
}

void Init_reader_ext()
{
  VALUE FastExcelReader = rb_define_module("FastExcelReader");
  VALUE Reader = rb_define_class_under(FastExcelReader, "Reader", rb_cObject);

  rb_define_singleton_method(Reader, "open", rb_reader_open, 1);
  rb_define_method(Reader, "sheet_names", rb_sheet_names, 0);
  rb_define_method(Reader, "each_row", rb_each_row, -1);
  rb_define_method(Reader, "close", rb_close, 0);
}

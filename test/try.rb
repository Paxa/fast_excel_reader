require_relative '../lib/fast_excel_reader'

require 'pp'
require 'time'

example_file = File.dirname(__FILE__) + "/../example_files/test_types.xlsx"

def read_fast(filename)
  rows = []
  reader = FastExcelReader::Reader.open(filename)

  reader.sheet_names.each do |sheet|
    reader.each_row(sheet: sheet) do |row|
      rows << row
    end
  end

  rows
end

def read_roo(filename)
  require 'roo'
  rows = []
  excel = Roo::Spreadsheet.open(filename)

  excel.sheets.each do |sheet_name|
    excel.each_row_streaming(sheet: sheet_name) do |row|
      rows << row.map(&:value)
    end
  end

  rows
end

def read_creek(filename)
  require 'creek'
  rows = []
  creek = Creek::Book.new(filename)

  creek.sheets.each do |sheet|
    sheet.rows.each do |row|
      rows << row.values
    end
  end

  rows
end

pp read_fast(example_file)[0..3]
#puts
#pp read_roo(example_file)[0..3]
#puts
#pp read_creek(example_file)[0..3]
#puts

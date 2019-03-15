module FastExcelReader
  class Reader

    def read_first_lines(number = 1, sheet: nil)
      lines = []
      each_row(sheet: sheet) do |row|
        lines << row
        break if lines.size == number
      end
      lines
    end

  end
end

begin
  require 'fast_excel_reader/reader_ext'
rescue LoadError => error
  # try to load file locally, for development
  ext = RUBY_PLATFORM =~ /darwin/ ? 'bundle' : 'so'
  local_path = File.expand_path("../../ext/fast_excel_reader/reader_ext.#{ext}", __FILE__)
  if File.exist?(local_path)
    require_relative '../ext/fast_excel_reader/reader_ext'
  else
    raise error
  end
end

module FastExcelReader
end

begin
  require 'fast_excel_reader/reader_ext'
rescue LoadError => error
  # try to load file locally, for development
  local_path = File.expand_path('../../ext/fast_excel_reader/reader_ext.bundle', __FILE__)
  if File.exist?(local_path)
    require_relative '../ext/fast_excel_reader/reader_ext'
  else
    raise error
  end
end

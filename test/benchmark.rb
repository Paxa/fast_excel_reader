require_relative 'lib/fast_excel_reader'

require 'bundler/setup'
require 'process_memory'
require 'roo'
require 'creek'

def measure_memory(title)
  puts "Running test: #{title}"
  start_time = Time.now.to_f
  recorder = ProcessMemory.start_recording(0.5) do
    puts recorder.print(title) if recorder
  end
  yield
  puts recorder.print("Done!")
  recorder.stop
  puts recorder.report_per_second_pretty
  puts "Complete in #{(Time.now.to_f - start_time).round(3)} sec"
  puts
end

#test_file = File.dirname(__FILE__) + "/test_dates.xlsx"
test_file = "/Users/pavel/Downloads/201903_08_R5401_70002_08032019.xlsx"

measure_memory("fast_excel_reader") do
  reader = FastExcelReader::Reader.open(test_file)

  rows = 0

  sheet_names = reader.sheet_names
  sheet_names.each do |sheet|
    reader.each_row(sheet: sheet) do |row|
      rows += 1
      p rows if rows % 10000 == 0
    end
  end

  p [:rows, rows]
end

measure_memory("creek") do
  creek = Creek::Book.new(test_file)

  rows = 0

  creek.sheets.each do |sheet|
    sheet.rows.each do |row|
      rows += 1
      p rows if rows % 10000 == 0
    end
  end
end

measure_memory("roo") do
  excel = Roo::Spreadsheet.open(test_file)

  rows = 0

  sheet_names = excel.sheets
  sheet_names.each do |sheet_name|
    excel.each_row_streaming(sheet: sheet_name) do |row|
      rows += 1
      p rows if rows % 10000 == 0
    end
  end

  p [:rows, rows]
end

desc "Sync github.com/Paxa/xlsxio to ext/fast_excel_reader/xlsxio"
task :sync_xlsxio do
  target_path = "./ext/fast_excel_reader/xlsxio"
  require 'fileutils'
  FileUtils.rm_rf(target_path)
  system("git clone --depth 10 git@github.com:Paxa/xlsxio.git #{target_path}")
  Dir.chdir(target_path) do
    system("git show --pretty='format:%cd %h' --date=iso --quiet > version.txt")
    FileUtils.rm_rf("./.git")
    FileUtils.rm_rf("./CMake")
    FileUtils.rm_rf("./doc")
  end
end

require 'rake/testtask'

Rake::TestTask.new do |test|
  test.test_files = Dir.glob('test/**/*_test.rb')
end

#task :default => :test

desc "Compile libxlsxwriter shared library"
task :compile do
  %x{
    cd ext/fast_excel_reader
    ruby ./extconf.rb
    make
  }
end

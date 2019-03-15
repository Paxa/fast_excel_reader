require 'mkmf'

# detect xlsxio path with fallback to local dir
local_xlsxio = File.expand_path(File.dirname(__FILE__) + "/xlsxio")
xlsxio_inc, xlsxio_lib = dir_config('xlsxio', local_xlsxio + "/include", local_xlsxio)

if xlsxio_lib == local_xlsxio
  puts "Building local xlsxio..."
  find_library('minizip', 'unzOpen')
  find_library('expat', 'XML_ParserCreate')

  Dir.chdir(local_xlsxio) do
    if RUBY_PLATFORM =~ /darwin/
      ENV['XLSXIOREAD_LDFLAGS'] = "-install_name #{local_xlsxio}/libxlsxio_read.dylib"
    end
    system "make shared-lib"
  end
end

find_library('xlsxio_read', 'xlsxioread_open', xlsxio_lib)
have_library('xlsxio_read', 'xlsxioread_open', 'xlsxio_read.h')

create_makefile('ext/reader_ext')

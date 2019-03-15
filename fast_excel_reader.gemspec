Gem::Specification.new do |s|
  s.name        = "fast_excel_reader"
  s.version     = "0.1.0"
  s.author      = ["Pavel Evstigneev"]
  s.email       = ["pavel.evst@gmail.com"]
  s.homepage    = "https://github.com/paxa/fast_excel_reader"
  s.summary     = %q{Ultra Fast Excel Reader}
  s.description = "Wrapper for xlsxio library"
  s.license     = 'MIT'
  s.required_ruby_version = '~> 2.0'

  s.files       = `git ls-files`.split("\n")
  s.test_files  = []

  s.require_paths = ["lib"]
  s.extensions = ["ext/fast_excel_reader/extconf.rb"]
end

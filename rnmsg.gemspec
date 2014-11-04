# -*- encoding: utf-8 -*

Gem::Specification.new do |s|
  s.name          = 'rnmsg'
  s.version       = '0.1.0'
  s.license       = 'MIT'
  s.author        = 'Alex Brem'
  s.email         = 'alex@fluktuation.net'
  s.homepage      = 'https://github.com/localhost/rnmsg'
  s.summary       = %w{ruby nanomsg}
  s.description   = %w{ruby support for the native nanomsg library}

  s.files         = [
    'README.md', 'LICENSE', 'Makefile', 'Gemfile', 'rnmsg.gemspec',
    'examples/req.rb', 'examples/rep.rb', 'ext/ruby/extconf.rb', 'ext/ruby/rubyext.c'
  ]
  s.require_paths = ['lib']

  s.extensions    = ["ext/ruby/extconf.rb"]

  s.has_rdoc      = false

  s.add_development_dependency 'rake'
end

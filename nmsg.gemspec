Gem::Specification.new do |s|
  s.name          = 'nmsg'
  s.version       = '0.1.2.pre'
  s.license       = 'MIT'
  s.author        = 'Alex Brem'
  s.email         = 'alex@fluktuation.net'
  s.homepage      = 'https://github.com/localhost/nmsg'
  s.summary       = %Q{native nanomsg binding}
  s.description   = %Q{native binding for the nanomsg c library}

  s.files         = [
    'README.md', 'LICENSE', 'Makefile', 'Gemfile', 'nmsg.gemspec',
    'examples/req.rb', 'examples/rep.rb', 'examples/push_pull.rb',
    'ext/nmsg/extconf.rb', 'ext/nmsg/rubyext.c', 'test/test_nmsg.rb'
  ]
  s.test_files    = ['test/test_nmsg.rb']

  s.require_paths = ['lib']
  s.extensions    = ["ext/nmsg/extconf.rb"]
  s.has_rdoc      = false
end

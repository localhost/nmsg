require 'mkmf'

$CFLAGS << " #{ENV['CFLAGS']}"
$LDFLAGS << " #{ENV['LDFLAGS']}"

have_header('ruby/thread.h') && have_func('rb_thread_call_without_gvl', 'ruby/thread.h') # Ruby 2.0+

have_library('nanomsg')
create_makefile('nmsg')

require 'mkmf'

$CFLAGS << " #{ENV['CFLAGS']}"
$LDFLAGS << " #{ENV['LDFLAGS']}"

have_library('nanomsg')
create_makefile('rnmsg')

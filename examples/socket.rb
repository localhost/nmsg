#!/usr/bin/env ruby

require_relative '../ext/ruby/rnmsg'

socket = Nanomsg::Socket.new Nanomsg::AF_SP, Nanomsg::NN_REP

puts Nanomsg::Socket.instance_methods.inspect

puts "fd: #{socket.get_fd}  sysfd: #{socket.get_sysfd}"

endpoint_id = socket.bind("tcp://127.0.0.1:4000")
puts "endpoint id: #{endpoint_id}"

trap("SIGINT") do
  puts "Exiting..."
  socket.close
  exit!
end

@semaphore = Mutex.new

def sem_puts(*args)
  @semaphore.synchronize { puts *args }
end

loop do
  ev = socket.poll Nanomsg::NN_POLLIN, 100
  raise "Error: nn_poll" if ev.nil?
  if ev
    data = socket.recv_msg
    sem_puts "=> HI [#{data}]"
    socket.send_msg data
    sem_puts "<= KTHXBAI [#{data}]"
    end
  end
end

socket.shutdown endpoint_id
socket.close

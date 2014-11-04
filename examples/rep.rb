#!/usr/bin/env ruby

require_relative '../ext/ruby/rnmsg'

socket = Nanomsg::Socket.new Nanomsg::AF_SP, Nanomsg::NN_REP
puts "fd: #{socket.get_fd}  sysfd: #{socket.get_sysfd}"

addr = ARGV.shift || "tcp://127.0.0.1:4000"
endpoint_id = socket.bind addr
puts "address: #{addr}  endpoint id: #{endpoint_id}"

trap("SIGINT") do
  puts "Exiting..."
  socket.shutdown endpoint_id
  socket.close
  exit!
end

loop do
  ev = socket.poll Nanomsg::NN_POLLIN, 1000
  raise "Error: nn_poll" if ev.nil?
  if ev
    data = socket.recv_msg
    puts "=> HAI [#{data}]"
    sleep Random.rand(1.0/Random.rand(1...100))
    socket.send_msg data.reverse
    puts "<= KTHXBAI [#{data}]"
  end
end

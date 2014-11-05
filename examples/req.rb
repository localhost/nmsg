#!/usr/bin/env ruby

require_relative '../ext/nmsg/nmsg'

socket = Nmsg::Socket.new Nmsg::AF_SP, Nmsg::NN_REQ
puts "fd: #{socket.get_fd}  sysfd: #{socket.get_sysfd}"

addr = ARGV.shift || "tcp://127.0.0.1:4000"
endpoint_id = socket.connect addr
puts "address: #{addr}  endpoint id: #{endpoint_id}"

trap("SIGINT") do
  puts "Exiting..."
  socket.shutdown endpoint_id
  socket.close
  exit!
end

sent = false
msg = ''
loop do
  puts "loop (sent: #{sent})"
  unless sent
    ev = socket.poll Nmsg::NN_POLLOUT, 1000
    next if ev.nil?
    if ev
      msg = (0...8).map { ('a'..'z').to_a[rand(26)] }.join
      sent = socket.send_msg_block msg
    end
    if sent
      puts "message sent: #{msg}"
    else
      sleep 1
    end
  else
    ev = socket.poll Nmsg::NN_POLLIN, 1000
    next if ev.nil?
    if ev
      data = socket.recv_msg
      puts "message received: #{data}" if data
      raise "Error: Wrong message! (expected '#{msg.reverse}' and got '#{data}')" unless data && data.reverse.eql?(msg)
      sent = false
    end
  end
end

#!/usr/bin/env ruby

require 'minitest/autorun'
require_relative '../ext/nmsg/nmsg'

class NmsgTestSocket < MiniTest::Unit::TestCase
  def setup
    @socket = Nmsg::Socket.new(Nmsg::AF_SP, Nmsg::NN_PAIR)
  end

  def test_socket
    assert @socket
  end

  def test_socket_option_linger
    ms = @socket.get_option(Nmsg::NN_SOL_SOCKET, Nmsg::NN_LINGER)
    assert_equal ms, 1000, "default linger time"

    res = @socket.set_option(Nmsg::NN_SOL_SOCKET, Nmsg::NN_LINGER, 0)
    assert res, "set linger"

    ms = @socket.get_option(Nmsg::NN_SOL_SOCKET, Nmsg::NN_LINGER)
    assert_equal ms, 0, "default linger time is 0"
  end

  def test_socket_option_sndbuf
    size = @socket.get_option(Nmsg::NN_SOL_SOCKET, Nmsg::NN_SNDBUF)
    assert_equal size, 131_072, "default send buffer size"

    res = @socket.set_option(Nmsg::NN_SOL_SOCKET, Nmsg::NN_SNDBUF, 262_144)
    assert res, "set send buffer size"

    size = @socket.get_option(Nmsg::NN_SOL_SOCKET, Nmsg::NN_SNDBUF)
    assert_equal size, 262_144, "send buffer size is 256kB"
  end

  def test_socket_option_rcvbuf
    size = @socket.get_option(Nmsg::NN_SOL_SOCKET, Nmsg::NN_RCVBUF)
    assert_equal size, 131_072, "default receive buffer size"

    res = @socket.set_option(Nmsg::NN_SOL_SOCKET, Nmsg::NN_RCVBUF, 262_144)
    assert res, "set receive buffer size"

    size = @socket.get_option(Nmsg::NN_SOL_SOCKET, Nmsg::NN_RCVBUF)
    assert_equal size, 262_144, "receive buffer size is 256kB"
  end
end

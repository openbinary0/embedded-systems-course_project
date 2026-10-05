proc rtt_poll {} {
    rtt start

    if {![catch {rtt channels}]} {
        echo "RTT started"
        rtt channels
        return
    }
    after 1000 rtt_poll
}

rtt setup 0x20000000 0xFFFF "SEGGER RTT"
rtt_poll
rtt server start 9090 0

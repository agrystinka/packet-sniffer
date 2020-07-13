User guide
==========

It is about how to use Packet Sniffer.

**Be ready that work with packet-sniffer requires root privileges.**


Before using any CLI, run the daemon. To do it, you might use:

``make run``  or ``./build/packet-sniffer``.


To interact with daemon packet-sniffer you could use following commands.

* ``./build/cli start`` - start writing sniffed packets into dump.txt.

* ``./build/cli stop`` - stop writing sniffed packets into dump.txt.

* ``./build/cli reset`` - reset file dump.txt.

* ``./build/cli show -a`` - show all info stored into dump.txt.

* ``./build/cli show -i [IP address]`` - show how many packets from/to some IP address are stored in dump.txt.

If you need some help, use  ``./build/cli --help`` or ``make runcli``.

While packet-sniffer works, it creates few files:

* ``dump.txt`` - here info about sniffed packets could be written, if you use command ``./build/cli start``.

* ``log.txt`` - here all daemon log messages are stored.

* ``daemon.txt`` - here the PID of daemon process is stored. It might make work with daemon easier.

If you want to kill daemon packet-sniffer process, please use ``SIGTERM`` only.

const assert   = require('chai').assert
const fs       = require('fs');
const util     = require('util');
const { stdout, stderr } = require('process');
const exec     = require('await-exec')
const readFilePromise = util.promisify(fs.readFile);

describe.only('Check for creating dump', () => {
    before(async () => {
        if(!process.env.ROOT_PASS) {
            throw new Error('ENTER ROOT PASSWORD')
        }
        await exec ('cd ../ && echo $ROOT_PASS | sudo -S ./packet-sniffer');

        await exec ('cd ../ && echo $ROOT_PASS | sudo -S ./cli start')

        await exec ('ping -c 2 8.8.8.8')
        await exec('cd ../ && echo $ROOT_PASS | sudo -S ./cli stop')

        // await exec('cd ../ && echo $ROOT_PASS | sudo -S kill $(cat daemon_id.txt)')

    });


    it('Positive: Check SRC IP is not empty array', async () => {
        const filePath = '../dump.txt';
        const dump = await readFilePromise(filePath, 'utf-8');
        const pattern = /(?:SRC\ IP\:\ )(?:[0-9]*\.){3}(?:[0-9]*)/g

        const arraysSRC_IP = dump.match(pattern);

        assert.notEqual(arraysSRC_IP.length, 0);
    });


	  it('Positive: Read File with PID', async () => {
        const password = process.env.ROOT_PASS

        exec(`cd ../ && echo $ROOT_PASS | sudo -S ./packet-sniffer`, (err, stdout, stderr) => {
            if (err) {
                console.error(stderr)
            }
            console.log(stdout);
        });


        const filePath = '../daemon_id.txt';
        const pid = await readFilePromise(filePath, 'utf-8');
        const pidToInt = parseInt(pid)

        assert.typeOf(pidToInt, 'number');
    });



    it('Positive: Dump file math a pattern', async () => {

        const filePath = '../dump.txt';
        const dump = await readFilePromise(filePath, 'utf-8');
        const pattern = /(?:-----Packet )(?:[\ a-zA-Z0-9\:]{24})\n(?:SRC IP\: )(?<SRC_IP>(?:[0-9]*\.){3}(?:[0-9]*))\n(?:DST\ IP\: )(?<DST_IP>(?:[0-9]*\.){3}(?:[0-9]*))\n(?:Protocol\: )(?<protocol>TCP|UDP|ICMP|unknown)\n(?:(?:(?:SRC port\: )(?<SRC_port>[0-9]*)\n(?:DST port\: )(?<DST_port>[0-9]*)\n)|(?<ICMP>Type\: [0-9]*[a-zA-Z\)\( ]*)|(?:))/gm
        const matchData = dump.match(pattern);
        assert.notEqual(matchData.length, 0);
    });
})

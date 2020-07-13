### Prepare
- Install node
```bash

curl -sL https://raw.githubusercontent.com/creationix/nvm/v0.33.11/install.sh -o install_nvm.sh

bash install_nvm.sh

source ~/.profile

nvm install 12.11.1
```

### Install node_modules
```bash
npm install
```

### Run test

```bash
make
export ROOT_PASS=${root_pass}
cd tests
npm run test
```

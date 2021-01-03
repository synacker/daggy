### Build
````docker build -t synacker2/daggy_linux_gcc8_debian:2.0.0 .````

### Run
````docker run --rm -it -u daggy -v $(pwd):/home/daggy/ synacker2/daggy_linux_gcc8_debian:2.0.0````

### Push
````docker push synacker2/daggy_linux_gcc8_debian:2.0.0````
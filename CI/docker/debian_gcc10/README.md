### Build
````docker build -t synacker2/daggy_linux_gcc10_debian:3.0.0 .````

### Run
````docker run --rm -u daggy -it -v $(pwd):/home/daggy synacker2/daggy_linux_gcc10_debian:3.0.0````

### Push
````docker push synacker2/daggy_linux_gcc10_debian:3.0.0````
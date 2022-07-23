### Build
````docker build -t synacker2/daggy_gcc10 .````

### Run
````docker run --rm -u daggy -it -v $(pwd):/home/daggy synacker2/daggy_gcc10````

### Push
````docker push synacker2/daggy_gcc10````
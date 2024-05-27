    FROM ubuntu

    RUN apt-get update && \
        apg-get install -y

    FROM gcc:latest

    COPY hello.c /docker_prueba/hello.c

    WORKDIR /docker_prueba/

    RUN gcc -o hello hello.c

    CMD [“./hello]

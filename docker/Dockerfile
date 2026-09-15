FROM linuxcontainers/debian-slim:12.5
RUN apt-get update
RUN apt-get install -y libgrpc++1.51
RUN apt-get clean
WORKDIR /usr/local
ARG PROGRAM_NAME
ENV PROGRAM_NAME=${PROGRAM_NAME}
COPY "${PROGRAM_NAME}" "bin/${PROGRAM_NAME}"
COPY "${PROGRAM_NAME}.json" "share/${PROGRAM_NAME}.json"
RUN chmod +x "bin/${PROGRAM_NAME}"
RUN echo "#!/bin/sh\ncd /usr/local\nexec bin/${PROGRAM_NAME} --config share/${PROGRAM_NAME}.json" > bin/entrypoint.sh
RUN chmod +x bin/entrypoint.sh
ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]

FROM hienanh/pybind11:v2.0
ENV DEBIAN_FRONTEND noninteractive

# Install necessary library
RUN apt update && \
    apt upgrade -y && \
    apt install --assume-yes software-properties-common

RUN cd /root && git clone https://github.com/datlt4/licensepp-openssl-flask.git && \
    cd /root/licensepp-openssl-flask/3rdparty/cryptopp && \
    make -j$(nproc) && \
    make install && \
    cd /root/licensepp-openssl-flask/3rdparty/licensepp && \
    mkdir -p build && \
    cd /root/licensepp-openssl-flask/3rdparty/licensepp/build && \
    cmake .. && \
    make -j$(nproc) && \
    make install && \
    cd /root/licensepp-openssl-flask/3rdparty/ripe && \
    mkdir -p build && \
    cd /root/licensepp-openssl-flask/3rdparty/ripe/build && \
    cmake .. && \
    make -j$(nproc) && \
    make install

RUN cd /root/licensepp-openssl-flask && \
    python3 -m pip install -r requirements.txt && \
    cd build && cmake .. && make -j$(nproc) && make install

# RUN
EXPOSE 6464
WORKDIR /root/licensepp-openssl-flask
CMD ["bash", "-c", "export LD_LIBRARY_PATH=/usr/local/lib && python3 server.py --host 0.0.0.0 --port 6464"]

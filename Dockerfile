# ---- Builder ----
    FROM alpine:3.20 AS builder

    ARG BUILD_TYPE=Release
    
    RUN apk add --no-cache \
        build-base ninja git
    
    # Pull cmake 4.x specifically from edge (main)
    RUN apk add --no-cache \
        --repository=https://dl-cdn.alpinelinux.org/alpine/edge/main \
        cmake
    
    WORKDIR /app
    COPY CMakeLists.txt .
    COPY src/CMakeLists.txt src/CMakeLists.txt
    COPY include/ include/
    COPY src/ src/
    COPY scene_boules.json scene_boules.json
    COPY scene_dna.json scene_dna.json
    COPY tests/ tests
    COPY main.cpp . 
    
    RUN cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
     && cmake --build build --config ${BUILD_TYPE}
    
    # ---- Runtime ----
    FROM alpine:3.20 AS runner
    RUN apk add --no-cache libstdc++
    WORKDIR /output

    COPY --from=builder /app/build/raytracer /usr/local/bin/raytracer
    COPY --from=builder /app/scene_boules.json /output/../scene_boules.json
    COPY --from=builder /app/scene_dna.json   /output/../scene_dna.json

    # Define volume for output files
    VOLUME ["/output"]

    ENTRYPOINT ["raytracer"]
    
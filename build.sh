g++ ./src/main.cpp -o exe.out -L./BulletPhysics/bin/ $(ls ./BulletPhysics/bin/ | grep '\.a$' | sed 's/lib/-l/' | sed 's/\.a//') -I./BulletPhysics/src/

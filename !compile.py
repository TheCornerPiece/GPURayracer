from subprocess import call
import os

sources = []
for filename in os.listdir('src'):
    if filename.endswith('.c'):
        sources.append('src/{}'.format(filename))

command = 'gcc -Iinclude main.c {} -o main.exe -lglfw3 -lopengl32 -lgdi32 -lmingw32'.format(' '.join(sources))
call(command)

call('main.exe')

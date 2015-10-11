require_relative 'src/plate_downloader'

SRC_DIR = 'src'
BIN_DIR = 'bin'
IMG_DIR = 'img'

CFLAGS  = `pkg-config --cflags opencv`.strip
LDFLAGS = `pkg-config --libs opencv`.strip
INCLUDE = ''#'-I/opt/local/include'

namespace :build do
  task :all do
    FileList[ File.join(SRC_DIR, '*.cpp')].each do |file|
      exec = File.join(BIN_DIR, File.basename(file, '.cpp'))
      sh "g++ #{CFLAGS} #{INCLUDE} -o #{exec} #{file} #{LDFLAGS}"
    end
  end
end

task :run do
  Dir.glob("img/*.jpg").each_with_index do |file, index|
    print "Processing image #{index + 1}...\n"
    print `./bin/plate_recognizer #{file}`
  end
end

task :setup do
  PlateDownloader.new( IMG_DIR ).download
end

task :default => ["build:all"]

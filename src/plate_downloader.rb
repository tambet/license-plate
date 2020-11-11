require 'open-uri'

class PlateDownloader

  def initialize( image_directory )
    @image_directory = image_directory
    @address  = 'https://raw.githubusercontent.com/openalpr/train-detector/master/eu/'
    @files    = %w{
      eu-1051.png eu-1052.png eu-1053.png eu-1054.png eu-1055.png eu-1056.png eu-1057.png eu-1058.png eu-1059.png eu-1060.png
    }
  end

  def download
    @files.each_with_index do | file_name, index |
      print "Downloading file #{file_name} ...\n"
      open(File.join(@image_directory, "vehicle-#{index+1}.jpg"),"w").write( open( [@address, file_name].join ).read )
    end
  end
end

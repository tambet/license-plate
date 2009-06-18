require 'open-uri'

class PlateDownloader

  def initialize( image_directory )
    @image_directory = image_directory
    @address  = 'http://bsd.ee/~innar/com/licence/data/'
    @files    = %w{
      2786635.jpg 2786639.jpg 2786641.jpg 3003006.jpg 3003010.jpg 3048109.jpg 3048113.jpg 3202448.jpg 
      3271964.jpg 3271968.jpg 3271976.jpg 3404253.jpg 3404258.jpg 3404260.jpg 3404263.jpg 3430028.jpg 
      3430030.jpg 3436511.jpg 3446780.jpg 3447829.jpg 3447833.jpg 3447925.jpg 3447929.jpg }
  end
  
  def download
    @files.each_with_index do | file_name, index |
      print "Downloading file #{file_name} ...\n"
      open(File.join(@image_directory, "vehicle-#{index+1}.jpg"),"w").write( open( [@address, file_name].join ).read )
    end 
  end
end
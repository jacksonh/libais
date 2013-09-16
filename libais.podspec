

Pod::Spec.new do |s|
  s.name         = 'libais'
  s.version      = '1.0.0'
  s.homepage     = 'https://github.com/rolker/libais'
  s.summary      = 'C++ decoder for Automatic Identification System for tracking ships and decoding maritime information.'
  s.source       = { :git => "https://github.com/jacksonh/libais.git", :tag => s.version.to_s }
  s.source_files = '*.cpp'
end


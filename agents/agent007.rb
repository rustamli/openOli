# 1 to 1 compare

a_str = ARGV[0]
es = 0

msg = ""

file_a = File.open(a_str).each { |line_a|
 
  line_a = line_a.strip
  line_a = line_a.chomp

  if line_a != "-1209"

    msg = "Error 007!"
    es = 1
    break;
  end
}


if msg==""
  msg = "Files are identical. ;)"
  es = 0
end

puts msg
exit es
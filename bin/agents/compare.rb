# 1 to 1 compare

detalisation = ARGV[0].to_i
a_str = ARGV[1] #problems
b_str = ARGV[2] #submits
es = 0

err_line = 0

msg = ""

file_b = File.open(b_str)

file_a = File.open(a_str).each { |line_a|


  err_line+=1
  line_b = file_b.readline;


  #operations
  if detalisation == 1
   
    line_a = line_a.strip
    line_b = line_b.strip

  elsif detalisation == 2
    line_a = line_a.strip
    line_b = line_b.strip
    line_a = line_a.chomp
    line_b = line_b.chomp
  end

  if line_a != line_b

    msg = "Error! On line "+err_line.to_s+"\n"+"Etalon's line is #{line_a}\n"+"Client's line is #{line_b}"
    es = 1
    break;
  end
}


if (msg=="")
  msg = "Files are identical."
  es = 0
end

#puts msg
exit es
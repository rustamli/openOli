# Gets sudo password from secret.txt

# Using modified version of FelixOJ judge code for Pascal, C and C++
# http://code.google.com/p/felixoj/

# Using javarun.rb for Java
# Is too simple and to be replaced with more advanced javarunner

if (ARGV[6]!="3")
  system "sudo ./run -e \"program\" -d \"submits/#{ARGV[4]}\" -I \"../../problems/#{ARGV[3]}/in_#{ARGV[5]}.txt\" -O \"submits/#{ARGV[4]}/out.txt\" -t #{ARGV[0]} -m #{ARGV[1]} -o #{ARGV[2]} -l #{ARGV[6]}  >\"submits/#{ARGV[4]}/runinfo.txt\""
else
  system "sudo ruby javarun.rb #{ARGV[0]} #{ARGV[1]} #{ARGV[2]} #{ARGV[3]} #{ARGV[4]} #{ARGV[5]} >\"submits/#{ARGV[4]}/runinfo.txt\""
end

exit $?.exitstatus

=begin
 * Name: sudorun.rb
 * openOli Runner
 * Author: Turan Rustamli
 * Date: 27.02.2010 23:58
 * License: MIT
=end

 tl = ARGV[0].to_i
 ml = ARGV[1].to_i
 ol = ARGV[2].to_i

 problem = ARGV[3]
 submit = ARGV[4]

 test = ARGV[5]
 
 tl = 1000 if tl < 1000

pid = fork do
    ol = 1000
    Process.setrlimit(Process::RLIMIT_FSIZE, ol) 
    
    Dir.chdir "submits/#{submit}"
    system "java Main <../../problems/#{problem}/in_#{test}.txt >stdout.txt"
    Dir.chdir "../.."

    es = $?.exitstatus
    es = 6 if es == 157
    puts es.to_s + " 0 0"
end

sleep (tl/1000).to_i
puts "3 0 0"
system "pkill -9 java"
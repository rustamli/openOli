# Gets sudo password from secret.txt
system "sudo ruby main.rb #{ARGV[0]} #{ARGV[1]} #{ARGV[2]} >phprun.inf"
exit $?.exitstatus

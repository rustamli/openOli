# openOli Default Checker

class DefaultChecker
    
    def check(submit)
        submit.compile

        if (submit.message!="")
            submit.report
        else
          check_compiled_submit(submit)
        end
    end

    def check_compiled_submit(submit)
          submit.get_problem_settings

          submit.num_tests.times do

            submit.run_program
            if (submit.message!="")
              submit.report
              break
            end

            submit.analyse_out
            if (submit.message!="")
              submit.report
              break
            end

            submit.next_test
          end

          if (submit.message == "")
            submit.accept
            submit.report
          end
    end
end

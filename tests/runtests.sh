echo "Running unit tests:"

for i in ./tests/*_tests.elf
do
    if test -f $i
    then
		mkfifo fifo || exit
		trap 'rm -f fifo' 0

		vba -N ./$i > fifo &
		
		status=false
		
		while read line; do
    		case $line in
        		END) echo "END found, breaking out."; break;;
				"ALL TESTS PASSED") status=true; echo "$line";;
        		*) echo "$line";;
    		esac
		done < fifo

        if $status
        then
            echo $i PASS
        else
            echo "ERROR in test $i: here's tests/tests.log"
            echo "------"
            tail ../tests/tests.log
            exit 1
        fi
    fi
done

echo ""

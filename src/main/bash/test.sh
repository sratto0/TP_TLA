#! /bin/bash

set -u

BASE_PATH="$(dirname "$0")/../../.."
cd "$BASE_PATH"

GREEN='\033[0;32m'
RED='\033[0;31m'
OFF='\033[0m'
STATUS=0
GENERATED_OUTPUT="schedule.html"

rm --force "$GENERATED_OUTPUT"

echo "Compiler should accept..."
echo ""

for test_path in src/test/c/accept/*; do
	test="$(basename "$test_path")"
	rm --force "$GENERATED_OUTPUT"
	LOGGING_LEVEL=ERROR ".build/Flex-Bison-Compiler" < "$test_path" >/dev/null 2>&1
	RESULT="$?"
	if [ "$RESULT" == "0" ]; then
		echo -e "    $test, ${GREEN}and it does${OFF} (status $RESULT)"
	else
		STATUS=1
		echo -e "    $test, ${RED}but it rejects${OFF} (status $RESULT)"
	fi
	rm --force "$GENERATED_OUTPUT"
done
echo ""

echo "Compiler should reject..."
echo ""

for test_path in src/test/c/reject/*; do
	test="$(basename "$test_path")"
	rm --force "$GENERATED_OUTPUT"
	LOGGING_LEVEL=ERROR ".build/Flex-Bison-Compiler" < "$test_path" >/dev/null 2>&1
	RESULT="$?"
	if [ "$RESULT" == "1" ]; then
		echo -e "    $test, ${GREEN}and it does${OFF} (status $RESULT)"
	else
		STATUS=1
		echo -e "    $test, ${RED}but it accepts${OFF} (status $RESULT)"
	fi
	rm --force "$GENERATED_OUTPUT"
done
echo ""

echo "Compiler should generate the expected output..."
echo ""

for expected_path in src/test/c/expected/*; do
	test="$(basename "$expected_path")"
	rm --force "$GENERATED_OUTPUT"
	LOGGING_LEVEL=ERROR ".build/Flex-Bison-Compiler" < "src/test/c/accept/$test" >/dev/null 2>&1
	RESULT="$?"
	MATCHES=true
	if [ "$RESULT" != "0" ] || [ ! -f "$GENERATED_OUTPUT" ]; then
		MATCHES=false
	else
		while IFS= read -r expected_line || [ -n "$expected_line" ]; do
			expected_line="${expected_line%$'\r'}"
			if ! grep --fixed-strings --quiet "$expected_line" "$GENERATED_OUTPUT"; then
				MATCHES=false
				echo "        missing: $expected_line"
				break
			fi
		done < "$expected_path"
	fi
	if [ "$MATCHES" == "true" ]; then
		echo -e "    $test, ${GREEN}and it does${OFF}"
	else
		STATUS=1
		echo -e "    $test, ${RED}but an expected HTML fragment is missing${OFF}"
	fi
	rm --force "$GENERATED_OUTPUT"
done
echo ""

echo "Compiler should not generate output without print statement..."
echo ""

for test_path in src/test/c/no-output/*; do
	test="$(basename "$test_path")"
	rm --force "$GENERATED_OUTPUT"
	LOGGING_LEVEL=ERROR ".build/Flex-Bison-Compiler" < "$test_path" >/dev/null 2>&1
	RESULT="$?"
	if [ "$RESULT" == "0" ] && [ ! -f "$GENERATED_OUTPUT" ]; then
		echo -e "    $test, ${GREEN}and it does not${OFF} (status $RESULT)"
	else
		STATUS=1
		echo -e "    $test, ${RED}but it generates output or rejects${OFF} (status $RESULT)"
	fi
	rm --force "$GENERATED_OUTPUT"
done
echo ""

echo "All done."
rm --force "$GENERATED_OUTPUT"
exit $STATUS

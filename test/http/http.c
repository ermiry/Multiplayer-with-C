#include <stdlib.h>
#include <stdio.h>

#include "http.h"

const char *private_key = { "./test/http/keys/key.key" };
const char *public_key = { "./test/http/keys/key.pub" };

const char *base_token = { "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJpYXQiOjE2MDc1NTU2ODYsImlkIjoiMTYwNzU1NTY4NiIsIm5hbWUiOiJFcmljayBTYWxhcyIsInJvbGUiOiJjb21tb24iLCJ1c2VybmFtZSI6ImVybWlyeSJ9.QIEZXo8vkthUPQzjozQNJ8P5ZTxbA6w2OWjeVplWVB2cs1ZySTRMvZ6Dh8dDe-CAHH4P9zcJumJIR0LWxZQ63O61c-cKZuupaPiI9qQaqql30oBnywznWCDkkXgkoTh683fSxoFR71Gzqp5e41mX-KxopH-Kh4Bz3eM3d27p-8tKOUGUlVk1AgWfGU2LFfTu1ZLkVYwo4ceGxHdntS1C_6IiutG8TLFjuKixgujIhK0ireG1cfAs7uvtGhWLXokLpvTbIrrjKSKLEjCcPh_yPpWeay0Y4yV1e5zSQKHiG7ry0D3qDWJcfHtjNLjAFb93TnpXtoXhftq_uL4d7BV16Y9ssQ5MpNoEh3bxHBgaCHOCGuCAVJfdDsRBT5C60_jme7S1utQ4qkpA8YbAoYKi58yGkHxnnYYbaYFQYSykbsYPFZ-4dieFlaIc5-m-vymwG--AMjfhu8Sdm0V0xA3Vq_9FzUIfXvo32-k0eH4QGPX6W8-FinR_Lj-Zj29mXfgALpTlF-m7Sbo3hVebnpwAFKZR495UBLL8B8u3MHB9XQxX6z4fHAxv5Nkftr4ShGRWwI_yVD73sLq44V9FEBQksoXLEBm5RqRC8qvhoxOIzbEYokd6MWLak1sZRUfIIqu65fERA5sMjs9JRnlYu7twP3Fk_VPy-tZBXXA7KTc3lkE" };
const char *bearer_token = { "Bearer eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJpYXQiOjE2MDc1NTU2ODYsImlkIjoiMTYwNzU1NTY4NiIsIm5hbWUiOiJFcmljayBTYWxhcyIsInJvbGUiOiJjb21tb24iLCJ1c2VybmFtZSI6ImVybWlyeSJ9.QIEZXo8vkthUPQzjozQNJ8P5ZTxbA6w2OWjeVplWVB2cs1ZySTRMvZ6Dh8dDe-CAHH4P9zcJumJIR0LWxZQ63O61c-cKZuupaPiI9qQaqql30oBnywznWCDkkXgkoTh683fSxoFR71Gzqp5e41mX-KxopH-Kh4Bz3eM3d27p-8tKOUGUlVk1AgWfGU2LFfTu1ZLkVYwo4ceGxHdntS1C_6IiutG8TLFjuKixgujIhK0ireG1cfAs7uvtGhWLXokLpvTbIrrjKSKLEjCcPh_yPpWeay0Y4yV1e5zSQKHiG7ry0D3qDWJcfHtjNLjAFb93TnpXtoXhftq_uL4d7BV16Y9ssQ5MpNoEh3bxHBgaCHOCGuCAVJfdDsRBT5C60_jme7S1utQ4qkpA8YbAoYKi58yGkHxnnYYbaYFQYSykbsYPFZ-4dieFlaIc5-m-vymwG--AMjfhu8Sdm0V0xA3Vq_9FzUIfXvo32-k0eH4QGPX6W8-FinR_Lj-Zj29mXfgALpTlF-m7Sbo3hVebnpwAFKZR495UBLL8B8u3MHB9XQxX6z4fHAxv5Nkftr4ShGRWwI_yVD73sLq44V9FEBQksoXLEBm5RqRC8qvhoxOIzbEYokd6MWLak1sZRUfIIqu65fERA5sMjs9JRnlYu7twP3Fk_VPy-tZBXXA7KTc3lkE" };

int main (int argc, char **argv) {

	(void) printf ("Testing HTTP...\n");

	// unit tests
	http_tests_responses ();
	http_tests_jwt ();

	// integration tests
	http_tests_jwt_encode ();
	http_tests_jwt_decode ();

	http_tests_admin ();

	(void) printf ("\nDone with HTTP tests!\n\n");

	return 0;

}
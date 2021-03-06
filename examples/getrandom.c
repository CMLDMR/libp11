/* libp11 example code: getrandom.c
 *
 * This examply simply connects to your smart card and
 * asks for a few random bytes.
 *
 * Feel free to copy all of the code as needed.
 *
 */

#include <stdio.h>
#include <libp11.h>

int main(int argc, char *argv[])
{
	PKCS11_CTX *ctx;
	PKCS11_SLOT *slots, *slot;
	unsigned char random[10];
	int rc = 0, i, len;
	unsigned int nslots;

	if (argc != 2) {
		fprintf(stderr, "usage: getrandom /usr/lib/opensc-pkcs11.so\n");
		return 1;
	}

	/* new context */
	ctx = PKCS11_CTX_new();

	/* load pkcs #11 module */
	rc = PKCS11_CTX_load(ctx, argv[1]);
	if (rc) {
		fprintf(stderr, "loading pkcs11 engine failed: %s\n",
			ERR_reason_error_string(ERR_get_error()));
		rc = 1;
		goto nolib;
	}

	/* get information on all slots */
	rc = PKCS11_enumerate_slots(ctx, &slots, &nslots);
	if (rc < 0) {
		fprintf(stderr, "no slots available\n");
		rc = 2;
		goto noslots;
	}
	printf("%d slots available\n", nslots);

	/* get first slot with a token */
	slot = PKCS11_find_token(ctx, slots, nslots);
	if (slot == NULL || slot->token == NULL) {
		fprintf(stderr, "no token available\n");
		rc = 3;
		goto notoken;
	}
	printf("Slot manufacturer......: %s\n", slot->manufacturer);
	printf("Slot description.......: %s\n", slot->description);
	printf("Slot token label.......: %s\n", slot->token->label);
	printf("Slot token manufacturer: %s\n", slot->token->manufacturer);
	printf("Slot token model.......: %s\n", slot->token->model);
	printf("Slot token serialnr....: %s\n", slot->token->serialnr);

	/* get 10 random bytes */
	len = sizeof(random);
	rc = PKCS11_generate_random(slot, random, len);
	if (rc < 0) {
		fprintf(stderr, "generate_random failed: %s\n",
			ERR_reason_error_string(ERR_get_error()));
		rc = 4;
		goto norandom;
	}

	printf("\nRandom numbers generated by the token: ");
	for (i = 0; i < len; i++)
		printf("%02X ", random[i]);
	printf("\n");

	rc = 0;

norandom:
notoken:
	PKCS11_release_all_slots(ctx, slots, nslots);

noslots:
	PKCS11_CTX_unload(ctx);

nolib:
	PKCS11_CTX_free(ctx);
	return rc;
}

/* vim: set noexpandtab: */

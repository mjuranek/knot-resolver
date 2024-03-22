#include <libknot/packet/pkt.h>
#include <libknot/dname.h>
#include <ccan/json/json.h>
#include <contrib/cleanup.h>

#include "daemon/engine.h"
#include "lib/layer.h"
#include "lib/generic/lru.h"

void print_ctx_info(kr_layer_t *ctx) {
	if (ctx->state & KR_STATE_CONSUME) kr_log_debug(DEVEL, "State consume\n");
	if (ctx->state & KR_STATE_DONE) kr_log_debug(DEVEL, "State done\n");
	if (ctx->state & KR_STATE_FAIL) kr_log_debug(DEVEL, "State fail\n");
	if (ctx->state & KR_STATE_PRODUCE) kr_log_debug(DEVEL, "State produce\n");
	if (ctx->state & KR_STATE_YIELD) kr_log_debug(DEVEL, "State yield\n");
}

void print_pkt(knot_pkt_t *pkt) {
	if (pkt == NULL) {
		kr_log_debug(DEVEL, "pkt is null\n");
		return;
	}

	auto_free char *pkt_text = kr_pkt_text(pkt);
	kr_log_debug(DEVEL, "%s\n", pkt_text);
}

int finish(kr_layer_t *ctx) {
	kr_log_debug(DEVEL, "------ FINISH -----\n");
	print_ctx_info(ctx);

	kr_log_notice(DEVEL, "finish layer\n");

	return ctx->state;
}

static int consume(kr_layer_t *ctx, knot_pkt_t *pkt)
{
	kr_log_debug(DEVEL, "------ CONSUME -----\n");
	print_ctx_info(ctx);
	print_pkt(pkt);

	return ctx->state;
}

static int produce(kr_layer_t *ctx, knot_pkt_t *pkt)
{
	kr_log_debug(DEVEL, "------ PRODUCE -----\n");
	print_ctx_info(ctx);
	print_pkt(pkt);

	auto_free char *qname_text = kr_dname_text(knot_pkt_qname(pkt));

	kr_log_debug(DEVEL, "DNAME %s\n", qname_text);

	if (ctx->req != NULL) {
		if (ctx->req->qsource.addr != NULL) {
			char * str_addr = kr_straddr(ctx->req->qsource.addr);
			char ipaddr[INET6_ADDRSTRLEN + 1];
			uint16_t port;
			kr_straddr_split(str_addr, ipaddr, &port);
			kr_log_notice(DEVEL, "SRC addr %s\n", ipaddr);
			if (strcmp(ipaddr, "127.0.0.1") == 0) {
				kr_log_notice(DEVEL, "127.0.0.1 -> Discarding request\n");
				ctx->req->options.NO_ANSWER = true;
				ctx->req->state = KR_STATE_FAIL;
				return KR_STATE_FAIL;
			}
		}
	}

	if (strcmp(qname_text, "youtube.com.") == 0) {
		kr_log_notice(DEVEL, "youtube.com query -> discarding packet\n");
		ctx->req->options.NO_ANSWER = true;
		ctx->req->state = KR_STATE_FAIL;
		return KR_STATE_FAIL;
	}

	return ctx->state;
}

static int reset(kr_layer_t *ctx)
{
	kr_log_debug(DEVEL, "------ RESET -----\n");
	print_ctx_info(ctx);

	return ctx->state;
}

static int begin(kr_layer_t *ctx)
{
	kr_log_debug(DEVEL, "------ BEGIN -----\n");
	print_ctx_info(ctx);

	return ctx->state;
}

static int answer_finalize(kr_layer_t *ctx)
{
	kr_log_debug(DEVEL, "------ ANSWER_FINALIZE -----\n");
	print_ctx_info(ctx);

	return ctx->state;
}

static int checkout(kr_layer_t *ctx, knot_pkt_t *packet, struct sockaddr *dst, int type)
{
	kr_log_debug(DEVEL, "------ CHECKOUT -----\n");
	print_ctx_info(ctx);
	print_pkt(packet);

	return ctx->state;
}

KR_EXPORT
int testmodule_init(struct kr_module *module)
{
	static kr_layer_api_t layer = {
		.finish = &finish,
		.consume = &consume,
		.produce = &produce,
		.reset = &reset,
		.begin = &begin,
		.answer_finalize = &answer_finalize,
		.checkout = &checkout,
	};
	layer.data = module;
	module->layer = &layer;

	return kr_ok();
}

KR_EXPORT
int testmodule_deinit(struct kr_module *module)
{
	return kr_ok();
}

KR_MODULE_EXPORT(testmodule)

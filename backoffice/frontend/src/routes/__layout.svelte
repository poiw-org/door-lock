<script lang="ts">
	import Header from '$lib/header/Header.svelte';
	import "../app.postcss";
	import * as Sentry from "@sentry/browser";
import { Integrations } from "@sentry/tracing";

Sentry.init({
  dsn: "https://88ac4e151ac24f4fbfc1496a088f527a@o350531.ingest.sentry.io/5916719",
  integrations: [new Integrations.BrowserTracing()],

  // Set tracesSampleRate to 1.0 to capture 100%
  // of transactions for performance monitoring.
  // We recommend adjusting this value in production
  tracesSampleRate: 1.0,
});
</script>

<Header>

<main>
	<slot></slot>
</main>


<style>

</style>
</Header>
<slot></slot>
<script>
import { onMount } from "svelte";
import auth from "../lib/auth/authService"
import Loading from "../lib/loading.svelte"
import API from "../lib/api"
let errorMessage = ""

onMount(async () => {
let auth0  = await auth.createClient();


try {
    try{
        await auth0.handleRedirectCallback();
    }catch(e) {
    }

    //logged in. you can get the user profile like this:
    const user = await auth0.getUser();
    if(typeof user == "undefined") await auth.login(auth0)
}catch (e) {
    errorMessage = e.message;
}

});

</script>

{#if !errorMessage}
    <span class="flex pb-20"><Loading/></span>
{/if}
{#if errorMessage}
    <span class="flex px-10 pt-20 w-full justify-center items-center flex-col gap-6">
        <span>An error occurred while trying to login. Try to clear your session data.</span>
        <p class="text-gray-300">Error: {errorMessage}</p>
    </span>
{/if}

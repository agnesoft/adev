<template>
    
<header class="page-header">
    <router-link :to="`/${$i18n.locale}`" class="logo">ADbStudio</router-link>
</header>
  
</template>

<script>
export default {
    name: "MainHeader"
}
</script>

<style scoped>
    .page-header {
        --button-size: 2.3rem;
        display: grid;
        padding: 20px;
        gap: 20px;
        grid-template-columns: minmax(5rem,1fr);
        grid-template-areas:
            'logo';
        background-color: white;
        padding: 1rem 1rem 0 1rem;
        border-bottom: 1px solid var(--light-primary-color);
        box-shadow: 0 2px 4px rgba(0,0,0,.0622169);
    }
    .logo{
        color: var(--secondary-color);
        font-size: 2rem;
        font-weight: 700;
        grid-area: logo;
        font-family: var(--primary-font);
    }
    .logo:hover{
        opacity: 0.7;
    }
</style>

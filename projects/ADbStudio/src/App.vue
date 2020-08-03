<template>
    <div id="app">    
        <MainHeader id="main-header" />
        
        <router-view  id="main-container"/>

        <MainFooter id="main-footer" />
    </div>
</template>


<script>
import MainHeader from "./components/layout/MainHeader";
import MainFooter from "./components/layout/MainFooter";


export default {
    name: 'app',
    components: {
        MainHeader,
        MainFooter
    },
}
</script>

<style>
:root{
    --primary-color: #ffa45c;
    --dark-color:#5d5d5a;
    --light-color:#fff4e3;
    --secondary-color:#ac3f21;
    --light-primary-color: #ffcdab;
    --light-grey: #ececec;

    --primary-font: 'Helvetica', 'Arial', sans-serif;

    --button-size: 2.4rem;
    --gradient-underline: linear-gradient(90deg, transparent 0%,var(--light-color) 30%,var(--light-color) 70%,transparent 100%);

    --border-radius: 0.4rem;
    --border-width: 0.25rem;

    --transition-ease: 0.5s ease;
    --transition-opacity: opacity var(--transition-ease);

    --grid-gap: 0.4rem;
    --menu-speed: 0.6s ease;

    /* --pullout-transform-origin: top left;
    --pullout-transform: scaleY(0);
    --pullout-transition: all 0.4s ease; */

}

*{
    box-sizing: border-box;
}

body{
    padding: 0;
    margin: 0;
}

a{
    text-decoration: none;
    transition: var(--transition-opacity);
}
ul{
    list-style-type: none;
}

/* implementation for old browsers*/
article, aside, footer, header, nav, section {
    display: block;
}

#app{
    background-color: var(--light-color);
    display: grid;
    grid-template-columns: 1fr;
    grid-template-rows: max-content minmax(max-content,1fr) max-content;
    grid-template-areas: 
        'header'
        'container'
        'footer'
        ;
    min-height: calc(100vh);
    grid-gap: 0;
    font-family: var(--primary-font);
}
#main-container{
    --wrapper-margin: 0.3rem;
    margin: var(--wrapper-margin);
    background-color: var(--light-color);
    padding: var(--grid-gap);
}
#main-header{
  grid-area: header;
}
#left-panel{
  grid-area: sidebar;
}
#main-content{
  grid-area: main;
}
#main-footer{
  grid-area: footer;
}
</style>

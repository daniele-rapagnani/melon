(window.webpackJsonp=window.webpackJsonp||[]).push([[7],{100:function(e,a,t){"use strict";t.d(a,"a",(function(){return p})),t.d(a,"b",(function(){return m}));var n=t(0),r=t.n(n);function l(e,a,t){return a in e?Object.defineProperty(e,a,{value:t,enumerable:!0,configurable:!0,writable:!0}):e[a]=t,e}function o(e,a){var t=Object.keys(e);if(Object.getOwnPropertySymbols){var n=Object.getOwnPropertySymbols(e);a&&(n=n.filter((function(a){return Object.getOwnPropertyDescriptor(e,a).enumerable}))),t.push.apply(t,n)}return t}function i(e){for(var a=1;a<arguments.length;a++){var t=null!=arguments[a]?arguments[a]:{};a%2?o(Object(t),!0).forEach((function(a){l(e,a,t[a])})):Object.getOwnPropertyDescriptors?Object.defineProperties(e,Object.getOwnPropertyDescriptors(t)):o(Object(t)).forEach((function(a){Object.defineProperty(e,a,Object.getOwnPropertyDescriptor(t,a))}))}return e}function c(e,a){if(null==e)return{};var t,n,r=function(e,a){if(null==e)return{};var t,n,r={},l=Object.keys(e);for(n=0;n<l.length;n++)t=l[n],a.indexOf(t)>=0||(r[t]=e[t]);return r}(e,a);if(Object.getOwnPropertySymbols){var l=Object.getOwnPropertySymbols(e);for(n=0;n<l.length;n++)t=l[n],a.indexOf(t)>=0||Object.prototype.propertyIsEnumerable.call(e,t)&&(r[t]=e[t])}return r}var b=r.a.createContext({}),s=function(e){var a=r.a.useContext(b),t=a;return e&&(t="function"==typeof e?e(a):i(i({},a),e)),t},p=function(e){var a=s(e.components);return r.a.createElement(b.Provider,{value:a},e.children)},u={inlineCode:"code",wrapper:function(e){var a=e.children;return r.a.createElement(r.a.Fragment,{},a)}},d=r.a.forwardRef((function(e,a){var t=e.components,n=e.mdxType,l=e.originalType,o=e.parentName,b=c(e,["components","mdxType","originalType","parentName"]),p=s(t),d=n,m=p["".concat(o,".").concat(d)]||p[d]||u[d]||l;return t?r.a.createElement(m,i(i({ref:a},b),{},{components:t})):r.a.createElement(m,i({ref:a},b))}));function m(e,a){var t=arguments,n=a&&a.mdxType;if("string"==typeof e||n){var l=t.length,o=new Array(l);o[0]=d;var i={};for(var c in a)hasOwnProperty.call(a,c)&&(i[c]=a[c]);i.originalType=e,i.mdxType="string"==typeof e?e:n,o[1]=i;for(var b=2;b<l;b++)o[b]=t[b];return r.a.createElement.apply(null,o)}return r.a.createElement.apply(null,t)}d.displayName="MDXCreateElement"},63:function(e,a,t){"use strict";t.r(a),t.d(a,"frontMatter",(function(){return o})),t.d(a,"metadata",(function(){return i})),t.d(a,"rightToc",(function(){return c})),t.d(a,"default",(function(){return s}));var n=t(2),r=t(6),l=(t(0),t(100)),o={id:"variables",title:"Variables",sidebar_label:"Variables",slug:"/variables"},i={unversionedId:"variables",id:"variables",isDocsHomePage:!1,title:"Variables",description:"Melon is a dynamically typed language. This means that the programmer doesn't have to specify",source:"@site/docs/variables.md",slug:"/variables",permalink:"/melon/docs/variables",editUrl:"https://github.com/daniele-rapagnani/edit/master/docs/docs/variables.md",version:"current",sidebar_label:"Variables",sidebar:"someSidebar",previous:{title:"Introduction",permalink:"/melon/docs/"},next:{title:"Integer",permalink:"/melon/docs/types/integer"}},c=[{value:"Local",id:"local",children:[]},{value:"Global",id:"global",children:[]}],b={rightToc:c};function s(e){var a=e.components,t=Object(r.a)(e,["components"]);return Object(l.b)("wrapper",Object(n.a)({},b,t,{components:a,mdxType:"MDXLayout"}),Object(l.b)("p",null,"Melon is a dynamically typed language. This means that the programmer doesn't have to specify\nthe type of a variable in advance, instead a variable can hold any type of value at any given moment of the program's execution."),Object(l.b)("h2",{id:"local"},"Local"),Object(l.b)("p",null,"Local variables are declared using the ",Object(l.b)("inlineCode",{parentName:"p"},"let")," keyword followed by the ",Object(l.b)("strong",{parentName:"p"},"variable name"),"."),Object(l.b)("p",null,"Variable names can have any combination of number, letters and the character ",Object(l.b)("inlineCode",{parentName:"p"},"_")," but they ",Object(l.b)("strong",{parentName:"p"},"cannot")," start with a number."),Object(l.b)("pre",null,Object(l.b)("code",Object(n.a)({parentName:"pre"},{className:"language-js"}),'let myVariable = "Hello";\n')),Object(l.b)("p",null,"Variables declared with ",Object(l.b)("inlineCode",{parentName:"p"},"let")," are block-scoped:"),Object(l.b)("pre",null,Object(l.b)("code",Object(n.a)({parentName:"pre"},{className:"language-js"}),'let myVariable = "Hello";\n\nif (something) {\n    let myVariable = "I\'m alive only inside this block!";\n}\n')),Object(l.b)("h2",{id:"global"},"Global"),Object(l.b)("p",null,"Any variable that is read or written to without having been declared is considered to be a global variable:"),Object(l.b)("pre",null,Object(l.b)("code",Object(n.a)({parentName:"pre"},{className:"language-js"}),'myGlobalVar = "I\'m inside a global variable";\n')),Object(l.b)("p",null,"Global variables are shared by the entire ",Object(l.b)("strong",{parentName:"p"},"VM")," instance and can be accessed from anywhere."),Object(l.b)("p",null,"The language uses global variables to expose core modules to the programmer, such as the ",Object(l.b)("a",Object(n.a)({parentName:"p"},{href:"/melon/docs/modules/io_module"}),Object(l.b)("inlineCode",{parentName:"a"},"io"))," or ",Object(l.b)("a",Object(n.a)({parentName:"p"},{href:"/melon/docs/modules/fs_module"}),Object(l.b)("inlineCode",{parentName:"a"},"fs"))," module."))}s.isMDXComponent=!0}}]);
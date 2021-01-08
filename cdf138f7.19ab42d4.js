(window.webpackJsonp=window.webpackJsonp||[]).push([[34],{100:function(e,n,r){"use strict";r.d(n,"a",(function(){return p})),r.d(n,"b",(function(){return d}));var t=r(0),a=r.n(t);function o(e,n,r){return n in e?Object.defineProperty(e,n,{value:r,enumerable:!0,configurable:!0,writable:!0}):e[n]=r,e}function i(e,n){var r=Object.keys(e);if(Object.getOwnPropertySymbols){var t=Object.getOwnPropertySymbols(e);n&&(t=t.filter((function(n){return Object.getOwnPropertyDescriptor(e,n).enumerable}))),r.push.apply(r,t)}return r}function c(e){for(var n=1;n<arguments.length;n++){var r=null!=arguments[n]?arguments[n]:{};n%2?i(Object(r),!0).forEach((function(n){o(e,n,r[n])})):Object.getOwnPropertyDescriptors?Object.defineProperties(e,Object.getOwnPropertyDescriptors(r)):i(Object(r)).forEach((function(n){Object.defineProperty(e,n,Object.getOwnPropertyDescriptor(r,n))}))}return e}function u(e,n){if(null==e)return{};var r,t,a=function(e,n){if(null==e)return{};var r,t,a={},o=Object.keys(e);for(t=0;t<o.length;t++)r=o[t],n.indexOf(r)>=0||(a[r]=e[r]);return a}(e,n);if(Object.getOwnPropertySymbols){var o=Object.getOwnPropertySymbols(e);for(t=0;t<o.length;t++)r=o[t],n.indexOf(r)>=0||Object.prototype.propertyIsEnumerable.call(e,r)&&(a[r]=e[r])}return a}var b=a.a.createContext({}),l=function(e){var n=a.a.useContext(b),r=n;return e&&(r="function"==typeof e?e(n):c(c({},n),e)),r},p=function(e){var n=l(e.components);return a.a.createElement(b.Provider,{value:n},e.children)},m={inlineCode:"code",wrapper:function(e){var n=e.children;return a.a.createElement(a.a.Fragment,{},n)}},s=a.a.forwardRef((function(e,n){var r=e.components,t=e.mdxType,o=e.originalType,i=e.parentName,b=u(e,["components","mdxType","originalType","parentName"]),p=l(r),s=t,d=p["".concat(i,".").concat(s)]||p[s]||m[s]||o;return r?a.a.createElement(d,c(c({ref:n},b),{},{components:r})):a.a.createElement(d,c({ref:n},b))}));function d(e,n){var r=arguments,t=n&&n.mdxType;if("string"==typeof e||t){var o=r.length,i=new Array(o);i[0]=s;var c={};for(var u in n)hasOwnProperty.call(n,u)&&(c[u]=n[u]);c.originalType=e,c.mdxType="string"==typeof e?e:t,i[1]=c;for(var b=2;b<o;b++)i[b]=r[b];return a.a.createElement.apply(null,i)}return a.a.createElement.apply(null,r)}s.displayName="MDXCreateElement"},91:function(e,n,r){"use strict";r.r(n),r.d(n,"frontMatter",(function(){return i})),r.d(n,"metadata",(function(){return c})),r.d(n,"rightToc",(function(){return u})),r.d(n,"default",(function(){return l}));var t=r(2),a=r(6),o=(r(0),r(100)),i={id:"number",title:"Number",sidebar_label:"Number",slug:"/types/number"},c={unversionedId:"number",id:"number",isDocsHomePage:!1,title:"Number",description:"A Number value can represent a floating point number:",source:"@site/docs/number.md",slug:"/types/number",permalink:"/melon/docs/types/number",editUrl:"https://github.com/daniele-rapagnani/edit/master/docs/docs/number.md",version:"current",sidebar_label:"Number",sidebar:"someSidebar",previous:{title:"Integer",permalink:"/melon/docs/types/integer"},next:{title:"Boolean",permalink:"/melon/docs/types/bool"}},u=[{value:"Mixing numbers and integers",id:"mixing-numbers-and-integers",children:[]},{value:"VM Internals",id:"vm-internals",children:[]}],b={rightToc:u};function l(e){var n=e.components,r=Object(a.a)(e,["components"]);return Object(o.b)("wrapper",Object(t.a)({},b,r,{components:n,mdxType:"MDXLayout"}),Object(o.b)("p",null,"A ",Object(o.b)("inlineCode",{parentName:"p"},"Number")," value can represent a floating point number:"),Object(o.b)("pre",null,Object(o.b)("code",Object(t.a)({parentName:"pre"},{className:"language-js"}),"let myNumber = 10.0;\nmyNumber = 0.005;\nmyNumber = -5.456e3;\nmyNumber = 5e-5;\n")),Object(o.b)("p",null,"To distinguish a ",Object(o.b)("inlineCode",{parentName:"p"},"Number")," value from an ",Object(o.b)("inlineCode",{parentName:"p"},"Integer")," value, the decimal ",Object(o.b)("strong",{parentName:"p"},"dot separator")," should always be used:"),Object(o.b)("pre",null,Object(o.b)("code",Object(t.a)({parentName:"pre"},{className:"language-js"}),"let myInteger = 10;\nlet myNumber = 10.0;\n")),Object(o.b)("h2",{id:"mixing-numbers-and-integers"},"Mixing numbers and integers"),Object(o.b)("p",null,"Combining ",Object(o.b)("inlineCode",{parentName:"p"},"Number")," values and ",Object(o.b)("inlineCode",{parentName:"p"},"Integer")," values with basic operations work as you would expect: ",Object(o.b)("inlineCode",{parentName:"p"},"Integer")," values are promoted to ",Object(o.b)("inlineCode",{parentName:"p"},"Number")," values and the result is a ",Object(o.b)("inlineCode",{parentName:"p"},"Number")," value.\nHowever comparing ",Object(o.b)("inlineCode",{parentName:"p"},"Integer"),"s and ",Object(o.b)("inlineCode",{parentName:"p"},"Number"),"s is not allowed to avoid any ambiguity, you have to convert one to the other:"),Object(o.b)("pre",null,Object(o.b)("code",Object(t.a)({parentName:"pre"},{className:"language-js"}),"// Compares an integer and a number\nif (5 * 1.0 > 4.0) {\n}\n\n// Compares a number and an integer, rounding the integer\nif (math.round(5.6) > 4) {\n}\n\n// Compares a number and an integer discarding the fractional part\nif (math.floor(5.6) > 4) {\n}\n")),Object(o.b)("h2",{id:"vm-internals"},"VM Internals"),Object(o.b)("p",null,"A ",Object(o.b)("inlineCode",{parentName:"p"},"Number")," is internally represented as ",Object(o.b)("strong",{parentName:"p"},"double-precision 64-bit IEEE 754"),"."))}l.isMDXComponent=!0}}]);
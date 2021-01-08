(window.webpackJsonp=window.webpackJsonp||[]).push([[38],{100:function(e,t,n){"use strict";n.d(t,"a",(function(){return p})),n.d(t,"b",(function(){return b}));var r=n(0),o=n.n(r);function i(e,t,n){return t in e?Object.defineProperty(e,t,{value:n,enumerable:!0,configurable:!0,writable:!0}):e[t]=n,e}function s(e,t){var n=Object.keys(e);if(Object.getOwnPropertySymbols){var r=Object.getOwnPropertySymbols(e);t&&(r=r.filter((function(t){return Object.getOwnPropertyDescriptor(e,t).enumerable}))),n.push.apply(n,r)}return n}function c(e){for(var t=1;t<arguments.length;t++){var n=null!=arguments[t]?arguments[t]:{};t%2?s(Object(n),!0).forEach((function(t){i(e,t,n[t])})):Object.getOwnPropertyDescriptors?Object.defineProperties(e,Object.getOwnPropertyDescriptors(n)):s(Object(n)).forEach((function(t){Object.defineProperty(e,t,Object.getOwnPropertyDescriptor(n,t))}))}return e}function u(e,t){if(null==e)return{};var n,r,o=function(e,t){if(null==e)return{};var n,r,o={},i=Object.keys(e);for(r=0;r<i.length;r++)n=i[r],t.indexOf(n)>=0||(o[n]=e[n]);return o}(e,t);if(Object.getOwnPropertySymbols){var i=Object.getOwnPropertySymbols(e);for(r=0;r<i.length;r++)n=i[r],t.indexOf(n)>=0||Object.prototype.propertyIsEnumerable.call(e,n)&&(o[n]=e[n])}return o}var a=o.a.createContext({}),l=function(e){var t=o.a.useContext(a),n=t;return e&&(n="function"==typeof e?e(t):c(c({},t),e)),n},p=function(e){var t=l(e.components);return o.a.createElement(a.Provider,{value:t},e.children)},m={inlineCode:"code",wrapper:function(e){var t=e.children;return o.a.createElement(o.a.Fragment,{},t)}},d=o.a.forwardRef((function(e,t){var n=e.components,r=e.mdxType,i=e.originalType,s=e.parentName,a=u(e,["components","mdxType","originalType","parentName"]),p=l(n),d=r,b=p["".concat(s,".").concat(d)]||p[d]||m[d]||i;return n?o.a.createElement(b,c(c({ref:t},a),{},{components:n})):o.a.createElement(b,c({ref:t},a))}));function b(e,t){var n=arguments,r=t&&t.mdxType;if("string"==typeof e||r){var i=n.length,s=new Array(i);s[0]=d;var c={};for(var u in t)hasOwnProperty.call(t,u)&&(c[u]=t[u]);c.originalType=e,c.mdxType="string"==typeof e?e:r,s[1]=c;for(var a=2;a<i;a++)s[a]=n[a];return o.a.createElement.apply(null,s)}return o.a.createElement.apply(null,n)}d.displayName="MDXCreateElement"},95:function(e,t,n){"use strict";n.r(t),n.d(t,"frontMatter",(function(){return s})),n.d(t,"metadata",(function(){return c})),n.d(t,"rightToc",(function(){return u})),n.d(t,"default",(function(){return l}));var r=n(2),o=n(6),i=(n(0),n(100)),s={id:"system_module",title:"system",sidebar_label:"system",slug:"/modules/system_module"},c={unversionedId:"system_module",id:"system_module",isDocsHomePage:!1,title:"system",description:"This module exposes functions to query information about",source:"@site/docs/system_module.md",slug:"/modules/system_module",permalink:"/melon/docs/modules/system_module",editUrl:"https://github.com/daniele-rapagnani/edit/master/docs/docs/system_module.md",version:"current",sidebar_label:"system",sidebar:"someSidebar",previous:{title:"string",permalink:"/melon/docs/modules/string_module"},next:{title:"types",permalink:"/melon/docs/modules/types_module"}},u=[{value:"Functions",id:"functions",children:[{value:"getArchBits()",id:"getarchbits",children:[]},{value:"getMelonVersion()",id:"getmelonversion",children:[]}]}],a={rightToc:u};function l(e){var t=e.components,n=Object(o.a)(e,["components"]);return Object(i.b)("wrapper",Object(r.a)({},a,n,{components:t,mdxType:"MDXLayout"}),Object(i.b)("p",null,"This module exposes functions to query information about\nthe VM on which the current code is running."),Object(i.b)("h2",{id:"functions"},"Functions"),Object(i.b)("h3",{id:"getarchbits"},"getArchBits()"),Object(i.b)("p",null,"Returns the CPU architecture the interpreter was built for. "),Object(i.b)("p",null,Object(i.b)("strong",{parentName:"p"},"Returns:")," An integer with the value 32 or 64"),Object(i.b)("h3",{id:"getmelonversion"},"getMelonVersion()"),Object(i.b)("p",null,"Gets the version of the interpreter currently in use "),Object(i.b)("p",null,Object(i.b)("strong",{parentName:"p"},"Returns:")," A string representing the version number"))}l.isMDXComponent=!0}}]);
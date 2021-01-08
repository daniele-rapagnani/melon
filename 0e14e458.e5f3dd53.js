(window.webpackJsonp=window.webpackJsonp||[]).push([[5],{100:function(e,t,n){"use strict";n.d(t,"a",(function(){return u})),n.d(t,"b",(function(){return s}));var a=n(0),r=n.n(a);function o(e,t,n){return t in e?Object.defineProperty(e,t,{value:n,enumerable:!0,configurable:!0,writable:!0}):e[t]=n,e}function l(e,t){var n=Object.keys(e);if(Object.getOwnPropertySymbols){var a=Object.getOwnPropertySymbols(e);t&&(a=a.filter((function(t){return Object.getOwnPropertyDescriptor(e,t).enumerable}))),n.push.apply(n,a)}return n}function c(e){for(var t=1;t<arguments.length;t++){var n=null!=arguments[t]?arguments[t]:{};t%2?l(Object(n),!0).forEach((function(t){o(e,t,n[t])})):Object.getOwnPropertyDescriptors?Object.defineProperties(e,Object.getOwnPropertyDescriptors(n)):l(Object(n)).forEach((function(t){Object.defineProperty(e,t,Object.getOwnPropertyDescriptor(n,t))}))}return e}function b(e,t){if(null==e)return{};var n,a,r=function(e,t){if(null==e)return{};var n,a,r={},o=Object.keys(e);for(a=0;a<o.length;a++)n=o[a],t.indexOf(n)>=0||(r[n]=e[n]);return r}(e,t);if(Object.getOwnPropertySymbols){var o=Object.getOwnPropertySymbols(e);for(a=0;a<o.length;a++)n=o[a],t.indexOf(n)>=0||Object.prototype.propertyIsEnumerable.call(e,n)&&(r[n]=e[n])}return r}var p=r.a.createContext({}),i=function(e){var t=r.a.useContext(p),n=t;return e&&(n="function"==typeof e?e(t):c(c({},t),e)),n},u=function(e){var t=i(e.components);return r.a.createElement(p.Provider,{value:t},e.children)},d={inlineCode:"code",wrapper:function(e){var t=e.children;return r.a.createElement(r.a.Fragment,{},t)}},m=r.a.forwardRef((function(e,t){var n=e.components,a=e.mdxType,o=e.originalType,l=e.parentName,p=b(e,["components","mdxType","originalType","parentName"]),u=i(n),m=a,s=u["".concat(l,".").concat(m)]||u[m]||d[m]||o;return n?r.a.createElement(s,c(c({ref:t},p),{},{components:n})):r.a.createElement(s,c({ref:t},p))}));function s(e,t){var n=arguments,a=t&&t.mdxType;if("string"==typeof e||a){var o=n.length,l=new Array(o);l[0]=m;var c={};for(var b in t)hasOwnProperty.call(t,b)&&(c[b]=t[b]);c.originalType=e,c.mdxType="string"==typeof e?e:a,l[1]=c;for(var p=2;p<o;p++)l[p]=n[p];return r.a.createElement.apply(null,l)}return r.a.createElement.apply(null,n)}m.displayName="MDXCreateElement"},54:function(e,t,n){"use strict";n.r(t),n.d(t,"frontMatter",(function(){return l})),n.d(t,"metadata",(function(){return c})),n.d(t,"rightToc",(function(){return b})),n.d(t,"default",(function(){return i}));var a=n(2),r=n(6),o=(n(0),n(100)),l={id:"path_module",title:"path",sidebar_label:"path",slug:"/modules/path_module"},c={unversionedId:"path_module",id:"path_module",isDocsHomePage:!1,title:"path",description:"This module can be used to manipulate String values that represent",source:"@site/docs/path_module.md",slug:"/modules/path_module",permalink:"/melon/docs/modules/path_module",editUrl:"https://github.com/daniele-rapagnani/edit/master/docs/docs/path_module.md",version:"current",sidebar_label:"path",sidebar:"someSidebar",previous:{title:"object",permalink:"/melon/docs/modules/object_module"},next:{title:"random",permalink:"/melon/docs/modules/random_module"}},b=[{value:"Functions",id:"functions",children:[{value:"dirname(path)",id:"dirnamepath",children:[]},{value:"basename(path)",id:"basenamepath",children:[]},{value:"realpath(path)",id:"realpathpath",children:[]}]}],p={rightToc:b};function i(e){var t=e.components,n=Object(r.a)(e,["components"]);return Object(o.b)("wrapper",Object(a.a)({},p,n,{components:t,mdxType:"MDXLayout"}),Object(o.b)("p",null,"This module can be used to manipulate ",Object(o.b)("inlineCode",{parentName:"p"},"String")," values that represent\nfilesystem paths."),Object(o.b)("h2",{id:"functions"},"Functions"),Object(o.b)("h3",{id:"dirnamepath"},"dirname(path)"),Object(o.b)("p",null,"Returns the last directory component of the provided path  "),Object(o.b)("table",null,Object(o.b)("thead",{parentName:"table"},Object(o.b)("tr",{parentName:"thead"},Object(o.b)("th",Object(a.a)({parentName:"tr"},{align:null}),"Argument"),Object(o.b)("th",Object(a.a)({parentName:"tr"},{align:null}),"Description"))),Object(o.b)("tbody",{parentName:"table"},Object(o.b)("tr",{parentName:"tbody"},Object(o.b)("td",Object(a.a)({parentName:"tr"},{align:null}),Object(o.b)("strong",{parentName:"td"},"path")),Object(o.b)("td",Object(a.a)({parentName:"tr"},{align:null}),"The path from which the dirname will be extracted")))),Object(o.b)("p",null,Object(o.b)("strong",{parentName:"p"},"Returns:")," The extracted directory component or ",Object(o.b)("inlineCode",{parentName:"p"},"null")," on error"),Object(o.b)("h3",{id:"basenamepath"},"basename(path)"),Object(o.b)("p",null,"Returns the file component of the provided path  "),Object(o.b)("table",null,Object(o.b)("thead",{parentName:"table"},Object(o.b)("tr",{parentName:"thead"},Object(o.b)("th",Object(a.a)({parentName:"tr"},{align:null}),"Argument"),Object(o.b)("th",Object(a.a)({parentName:"tr"},{align:null}),"Description"))),Object(o.b)("tbody",{parentName:"table"},Object(o.b)("tr",{parentName:"tbody"},Object(o.b)("td",Object(a.a)({parentName:"tr"},{align:null}),Object(o.b)("strong",{parentName:"td"},"path")),Object(o.b)("td",Object(a.a)({parentName:"tr"},{align:null}),"The path from which the basename will be extracted")))),Object(o.b)("p",null,Object(o.b)("strong",{parentName:"p"},"Returns:")," The extracted file component or ",Object(o.b)("inlineCode",{parentName:"p"},"null")," on error"),Object(o.b)("h3",{id:"realpathpath"},"realpath(path)"),Object(o.b)("p",null,"Resolves any relative components or symbolic links of a given path and returns the real absolute path.  "),Object(o.b)("table",null,Object(o.b)("thead",{parentName:"table"},Object(o.b)("tr",{parentName:"thead"},Object(o.b)("th",Object(a.a)({parentName:"tr"},{align:null}),"Argument"),Object(o.b)("th",Object(a.a)({parentName:"tr"},{align:null}),"Description"))),Object(o.b)("tbody",{parentName:"table"},Object(o.b)("tr",{parentName:"tbody"},Object(o.b)("td",Object(a.a)({parentName:"tr"},{align:null}),Object(o.b)("strong",{parentName:"td"},"path")),Object(o.b)("td",Object(a.a)({parentName:"tr"},{align:null}),"The path to be transformed to a real absolute path")))),Object(o.b)("p",null,Object(o.b)("strong",{parentName:"p"},"Returns:")," The real path or ",Object(o.b)("inlineCode",{parentName:"p"},"null")," on error"))}i.isMDXComponent=!0}}]);
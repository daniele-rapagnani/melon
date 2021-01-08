(window.webpackJsonp=window.webpackJsonp||[]).push([[40],{100:function(e,t,n){"use strict";n.d(t,"a",(function(){return s})),n.d(t,"b",(function(){return j}));var r=n(0),a=n.n(r);function i(e,t,n){return t in e?Object.defineProperty(e,t,{value:n,enumerable:!0,configurable:!0,writable:!0}):e[t]=n,e}function l(e,t){var n=Object.keys(e);if(Object.getOwnPropertySymbols){var r=Object.getOwnPropertySymbols(e);t&&(r=r.filter((function(t){return Object.getOwnPropertyDescriptor(e,t).enumerable}))),n.push.apply(n,r)}return n}function o(e){for(var t=1;t<arguments.length;t++){var n=null!=arguments[t]?arguments[t]:{};t%2?l(Object(n),!0).forEach((function(t){i(e,t,n[t])})):Object.getOwnPropertyDescriptors?Object.defineProperties(e,Object.getOwnPropertyDescriptors(n)):l(Object(n)).forEach((function(t){Object.defineProperty(e,t,Object.getOwnPropertyDescriptor(n,t))}))}return e}function b(e,t){if(null==e)return{};var n,r,a=function(e,t){if(null==e)return{};var n,r,a={},i=Object.keys(e);for(r=0;r<i.length;r++)n=i[r],t.indexOf(n)>=0||(a[n]=e[n]);return a}(e,t);if(Object.getOwnPropertySymbols){var i=Object.getOwnPropertySymbols(e);for(r=0;r<i.length;r++)n=i[r],t.indexOf(n)>=0||Object.prototype.propertyIsEnumerable.call(e,n)&&(a[n]=e[n])}return a}var c=a.a.createContext({}),p=function(e){var t=a.a.useContext(c),n=t;return e&&(n="function"==typeof e?e(t):o(o({},t),e)),n},s=function(e){var t=p(e.components);return a.a.createElement(c.Provider,{value:t},e.children)},u={inlineCode:"code",wrapper:function(e){var t=e.children;return a.a.createElement(a.a.Fragment,{},t)}},d=a.a.forwardRef((function(e,t){var n=e.components,r=e.mdxType,i=e.originalType,l=e.parentName,c=b(e,["components","mdxType","originalType","parentName"]),s=p(n),d=r,j=s["".concat(l,".").concat(d)]||s[d]||u[d]||i;return n?a.a.createElement(j,o(o({ref:t},c),{},{components:n})):a.a.createElement(j,o({ref:t},c))}));function j(e,t){var n=arguments,r=t&&t.mdxType;if("string"==typeof e||r){var i=n.length,l=new Array(i);l[0]=d;var o={};for(var b in t)hasOwnProperty.call(t,b)&&(o[b]=t[b]);o.originalType=e,o.mdxType="string"==typeof e?e:r,l[1]=o;for(var c=2;c<i;c++)l[c]=n[c];return a.a.createElement.apply(null,l)}return a.a.createElement.apply(null,n)}d.displayName="MDXCreateElement"},97:function(e,t,n){"use strict";n.r(t),n.d(t,"frontMatter",(function(){return l})),n.d(t,"metadata",(function(){return o})),n.d(t,"rightToc",(function(){return b})),n.d(t,"default",(function(){return p}));var r=n(2),a=n(6),i=(n(0),n(100)),l={id:"json_module",title:"json",sidebar_label:"json",slug:"/modules/json_module"},o={unversionedId:"json_module",id:"json_module",isDocsHomePage:!1,title:"json",description:"This module provides built-in JSON serialization/deserialization support.",source:"@site/docs/json_module.md",slug:"/modules/json_module",permalink:"/melon/docs/modules/json_module",editUrl:"https://github.com/daniele-rapagnani/edit/master/docs/docs/json_module.md",version:"current",sidebar_label:"json",sidebar:"someSidebar",previous:{title:"io",permalink:"/melon/docs/modules/io_module"},next:{title:"math",permalink:"/melon/docs/modules/math_module"}},b=[{value:"Functions",id:"functions",children:[{value:"parse(jsonString)",id:"parsejsonstring",children:[]},{value:"parseFile(jsonPath)",id:"parsefilejsonpath",children:[]},{value:"stringify(value, prettify)",id:"stringifyvalue-prettify",children:[]}]}],c={rightToc:b};function p(e){var t=e.components,n=Object(a.a)(e,["components"]);return Object(i.b)("wrapper",Object(r.a)({},c,n,{components:t,mdxType:"MDXLayout"}),Object(i.b)("p",null,"This module provides built-in JSON serialization/deserialization support."),Object(i.b)("h2",{id:"functions"},"Functions"),Object(i.b)("h3",{id:"parsejsonstring"},"parse(jsonString)"),Object(i.b)("p",null,"Deserializes a given JSON from a string  "),Object(i.b)("table",null,Object(i.b)("thead",{parentName:"table"},Object(i.b)("tr",{parentName:"thead"},Object(i.b)("th",Object(r.a)({parentName:"tr"},{align:null}),"Argument"),Object(i.b)("th",Object(r.a)({parentName:"tr"},{align:null}),"Description"))),Object(i.b)("tbody",{parentName:"table"},Object(i.b)("tr",{parentName:"tbody"},Object(i.b)("td",Object(r.a)({parentName:"tr"},{align:null}),Object(i.b)("strong",{parentName:"td"},"jsonString")),Object(i.b)("td",Object(r.a)({parentName:"tr"},{align:null}),"A string representing a valid JSON")))),Object(i.b)("p",null,Object(i.b)("strong",{parentName:"p"},"Returns:")," The deserialized JSON or ",Object(i.b)("inlineCode",{parentName:"p"},"null")," if parsing failed."),Object(i.b)("h3",{id:"parsefilejsonpath"},"parseFile(jsonPath)"),Object(i.b)("p",null,"Reads and parses the provided JSON file.  "),Object(i.b)("table",null,Object(i.b)("thead",{parentName:"table"},Object(i.b)("tr",{parentName:"thead"},Object(i.b)("th",Object(r.a)({parentName:"tr"},{align:null}),"Argument"),Object(i.b)("th",Object(r.a)({parentName:"tr"},{align:null}),"Description"))),Object(i.b)("tbody",{parentName:"table"},Object(i.b)("tr",{parentName:"tbody"},Object(i.b)("td",Object(r.a)({parentName:"tr"},{align:null}),Object(i.b)("strong",{parentName:"td"},"jsonPath")),Object(i.b)("td",Object(r.a)({parentName:"tr"},{align:null}),"A valid path to a readable file")))),Object(i.b)("p",null,Object(i.b)("strong",{parentName:"p"},"Returns:")," The deserialized JSON or ",Object(i.b)("inlineCode",{parentName:"p"},"null")," if parsing failed."),Object(i.b)("h3",{id:"stringifyvalue-prettify"},"stringify(value, prettify)"),Object(i.b)("p",null,"Serializes a given value to a JSON string.   "),Object(i.b)("table",null,Object(i.b)("thead",{parentName:"table"},Object(i.b)("tr",{parentName:"thead"},Object(i.b)("th",Object(r.a)({parentName:"tr"},{align:null}),"Argument"),Object(i.b)("th",Object(r.a)({parentName:"tr"},{align:null}),"Description"))),Object(i.b)("tbody",{parentName:"table"},Object(i.b)("tr",{parentName:"tbody"},Object(i.b)("td",Object(r.a)({parentName:"tr"},{align:null}),Object(i.b)("strong",{parentName:"td"},"value")),Object(i.b)("td",Object(r.a)({parentName:"tr"},{align:null}),"The value that should be JSON serialized")),Object(i.b)("tr",{parentName:"tbody"},Object(i.b)("td",Object(r.a)({parentName:"tr"},{align:null}),Object(i.b)("strong",{parentName:"td"},"prettify")),Object(i.b)("td",Object(r.a)({parentName:"tr"},{align:null}),Object(i.b)("inlineCode",{parentName:"td"},"true")," If the output should be prettified, defaults to ",Object(i.b)("inlineCode",{parentName:"td"},"false"))))),Object(i.b)("p",null,Object(i.b)("strong",{parentName:"p"},"Returns:")," A JSON string representing ",Object(i.b)("inlineCode",{parentName:"p"},"value")," or ",Object(i.b)("inlineCode",{parentName:"p"},"null")," if serialization failed."))}p.isMDXComponent=!0}}]);
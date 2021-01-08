---
id: {{ id }}
title: {{ name }}
sidebar_label: {{ name }}
slug: /modules/{{ id }}
---

{{ description.summary | safe }}

{% if functions | length > 0 %}
## Functions
{% for func in functions %}
{% if func.signature %}
### {{ func.signature }}
{{ func.docs.summary | safe | replace("\n", " ") }}

{% if func.docs.args | length > 0 %}
| Argument | Description |
| -------- | ----------- |
{% for arg in func.docs.args %}| {% if arg.optional %} **{{ arg.name }}** &nbsp; <sub><sup>Optional</sup></sub> {% else %} **{{ arg.name }}** {% endif %} | {{ arg.description | safe }} |
{% endfor %}
{% endif %}
{% if func.docs.returns %}
**Returns:** {{ func.docs.returns.description }}
{% endif %}
{% endif %}
{% endfor %}
{% endif %}

{% if description.exports | length > 0 %}
## Properties

{% for export in description.exports %}
### {{ export.name }}
{{ export.description }}
{% endfor %}
{% endif %}
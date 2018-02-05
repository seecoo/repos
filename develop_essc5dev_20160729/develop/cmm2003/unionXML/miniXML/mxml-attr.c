/*
 * "$Id: mxml-attr.c 451 2014-01-04 21:50:06Z msweet $"
 *
 * Attribute support code for Mini-XML, a small XML-like file parsing library.
 *
 * Copyright 2003-2014 by Michael R Sweet.
 *
 * These coded instructions, statements, and computer programs are the
 * property of Michael R Sweet and are protected by Federal copyright
 * law.  Distribution and use rights are outlined in the file "COPYING"
 * which should have been included with this file.  If this file is
 * missing or damaged, see the license at:
 *
 *     http://www.msweet.org/projects.php/Mini-XML
 */

/*
 * Include necessary headers...
 */

#include "config.h"
#include "mxml.h"

/*
 * Local functions...
 */

static int	mxml_set_attr(mxml_cache_t *mxml_cache,mxml_node_t *node, const char *name,
		              char *value);


/*
 * 'mxmlElementDeleteAttr()' - Delete an attribute.
 *
 * @since Mini-XML 2.4@
 */

void
mxmlElementDeleteAttr(mxml_cache_t *mxml_cache,
		      mxml_node_t *node,/* I - Element */
                      const char  *name)/* I - Attribute name */
{
  int		i;			/* Looping var */
  mxml_attr_t	*attr;			/* Cirrent attribute */
  char		*ptr = NULL;


#ifdef DEBUG
  fprintf(stderr, "mxmlElementDeleteAttr(node=%p, name=\"%s\")\n",
          node, name ? name : "(null)");
#endif /* DEBUG */

 /*
  * Range check input...
  */

  if (!node || node->type != MXML_ELEMENT || !name)
    return;

 /*
  * Look for the attribute...
  */

  for (i = node->value.element.num_attrs, attr = node->value.element.attrs;
       i > 0;
       i --, attr ++)
  {
    //ptr = attr->name;
    ptr = mxml_cache->buffer + attr->name_offset;
    if (!strcmp(ptr, name))
    {
     /*
      * Delete this attribute...
      */

      //free(attr->name);
      //free(attr->value);

      i --;
      if (i > 0)
        memmove(attr, attr + 1, i * sizeof(mxml_attr_t));

      node->value.element.num_attrs --;

      //if (node->value.element.num_attrs == 0)
        //free(node->value.element.attrs);
      return;
    }
  }
}


/*
 * 'mxmlElementGetAttr()' - Get an attribute.
 *
 * This function returns NULL if the node is not an element or the
 * named attribute does not exist.
 */

const char *				/* O - Attribute value or NULL */
mxmlElementGetAttr(mxml_cache_t *mxml_cache,
		   mxml_node_t *node,	/* I - Element node */
                   const char  *name)	/* I - Name of attribute */
{
  int		i;			/* Looping var */
  mxml_attr_t	*attr;			/* Cirrent attribute */
  char		*ptr = NULL;


#ifdef DEBUG
  fprintf(stderr, "mxmlElementGetAttr(node=%p, name=\"%s\")\n",
          node, name ? name : "(null)");
#endif /* DEBUG */

 /*
  * Range check input...
  */

  if (!node || node->type != MXML_ELEMENT || !name)
    return (NULL);

 /*
  * Look for the attribute...
  */

  for (i = node->value.element.num_attrs, attr = node->value.element.attrs;
       i > 0;
       i --, attr ++)
  {
    //ptr = attr->name;
    ptr = mxml_cache->buffer + attr->name_offset;
    if (!strcmp(ptr, name))
    {
      //return (attr->value);
      return(mxml_cache->buffer + attr->value_offset);
    }
  }

 /*
  * Didn't find attribute, so return NULL...
  */

#ifdef DEBUG
  puts("    Returning NULL!\n");
#endif /* DEBUG */

  return (NULL);
}


/*
 * 'mxmlElementSetAttr()' - Set an attribute.
 *
 * If the named attribute already exists, the value of the attribute
 * is replaced by the new string value. The string value is copied
 * into the element node. This function does nothing if the node is
 * not an element.
 */

void
mxmlElementSetAttr(mxml_cache_t *mxml_cache,
		   mxml_node_t *node,	/* I - Element node */
                   const char  *name,	/* I - Name of attribute */
                   const char  *value)	/* I - Attribute value */
{
  //char	*valuec;			/* Copy of value */


#ifdef DEBUG
  fprintf(stderr, "mxmlElementSetAttr(node=%p, name=\"%s\", value=\"%s\")\n",
          node, name ? name : "(null)", value ? value : "(null)");
#endif /* DEBUG */

 /*
  * Range check input...
  */

  if (!node || node->type != MXML_ELEMENT || !name)
    return;
  /*
  if (value)
    valuec = strdup(value);
  else
    valuec = NULL;
  if (mxml_set_attr(node, name, valuec))
    free(valuec);
  */
  mxml_set_attr(mxml_cache,node, name, (char *)value);
}


/*
 * 'mxmlElementSetAttrf()' - Set an attribute with a formatted value.
 *
 * If the named attribute already exists, the value of the attribute
 * is replaced by the new formatted string. The formatted string value is
 * copied into the element node. This function does nothing if the node
 * is not an element.
 *
 * @since Mini-XML 2.3@
 */

void
mxmlElementSetAttrf(mxml_cache_t *mxml_cache,
		    mxml_node_t *node,	/* I - Element node */
                    const char  *name,	/* I - Name of attribute */
                    const char  *format,/* I - Printf-style attribute value */
		    ...)		/* I - Additional arguments as needed */
{
  va_list	ap;			/* Argument pointer */
  char		value[8192];			/* Value */
  int		len;


#ifdef DEBUG
  fprintf(stderr,
          "mxmlElementSetAttrf(node=%p, name=\"%s\", format=\"%s\", ...)\n",
          node, name ? name : "(null)", format ? format : "(null)");
#endif /* DEBUG */

 /*
  * Range check input...
  */

  if (!node || node->type != MXML_ELEMENT || !name || !format)
    return;

 /*
  * Format the value...
  */
  /*
  va_start(ap, format);
  value = _mxml_vstrdupf(format, ap);
  va_end(ap);

  if (!value)
    mxml_error("Unable to allocate memory for attribute '%s' in element %s!",
               name, node->value.element.name);
  else if (mxml_set_attr(node, name, value))
    free(value);
  */
  va_start(ap, format);
  len = vsnprintf(value, sizeof(value), format, ap);
  va_end(ap);

  if (len < sizeof(value))
    value[len] = 0;
  else
  {
    mxml_error("Unable to buffer size too small!");
    return;
  }
  mxml_set_attr(mxml_cache,node, name, value);
}


/*
 * 'mxml_set_attr()' - Set or add an attribute name/value pair.
 */

static int				/* O - 0 on success, -1 on failure */
mxml_set_attr(mxml_cache_t *mxml_cache,
	      mxml_node_t *node,	/* I - Element node */
              const char  *name,	/* I - Attribute name */
              char        *value)	/* I - Attribute value */
{
  int		i;			/* Looping var */
  mxml_attr_t	*attr;			/* New attribute */
  char		*ptr = NULL;


 /*
  * Look for the attribute...
  */

  for (i = node->value.element.num_attrs, attr = node->value.element.attrs;
       i > 0;
       i --, attr ++)
  {
    //ptr = attr->name;
    ptr = mxml_cache->buffer + attr->name_offset;
    if (!strcmp(ptr, name))
    {
     /*
      * Free the old value as needed...
      */

      /*
      if (attr->value)
        free(attr->value);

      attr->value = value;
      */
      //ptr = attr->value;
      ptr = mxml_cache->buffer + attr->value_offset;
      if (!strcmp(ptr, value))	// value is samle
      	return(0);
      else if (strlen(ptr) == strlen(value))	// value length is samle
      {
	memcpy(ptr,value,strlen(value));
      	return(0);
      }
      
      if (mxml_check_buffer_size(mxml_cache, strlen(value)))
      	return(-1);
      //attr->value = mxml_cache->bufptr;
      //strcpy(attr->value,value);
      attr->value_offset = mxml_cache->buf_offset;
      MXML_OFFSET_BUFPTR( mxml_cache , value , strlen(value) )

      return (0);
    }
  }

 /*
  * Add a new attribute...
  */
  /*
  if (node->value.element.num_attrs == 0)
    attr = malloc(sizeof(mxml_attr_t));
  else
    attr = realloc(node->value.element.attrs,
                   (node->value.element.num_attrs + 1) * sizeof(mxml_attr_t));

  if (!attr)
  {
    mxml_error("Unable to allocate memory for attribute '%s' in element %s!",
               name, node->value.element.name);
    return (-1);
  }
  node->value.element.attrs = attr;
  attr += node->value.element.num_attrs;
  
  if ((attr->name = strdup(name)) == NULL)
  {
    mxml_error("Unable to allocate memory for attribute '%s' in element %s!",
               name, node->value.element.name);
    return (-1);
  }

  //attr->value = value;
  */
  attr = node->value.element.attrs + node->value.element.num_attrs;
  if (mxml_check_buffer_size(mxml_cache, strlen(name)))
    return(-1);
  //attr->name = mxml_cache->bufptr;
  //strcpy(attr->name,name);
  attr->name_offset = mxml_cache->buf_offset;
  MXML_OFFSET_BUFPTR( mxml_cache , name , strlen(name) )
  
  if (mxml_check_buffer_size(mxml_cache, strlen(value)))
    return(-1);
  //attr->value = mxml_cache->bufptr;
  //strcpy(attr->value,value);
  attr->value_offset = mxml_cache->buf_offset;
  MXML_OFFSET_BUFPTR( mxml_cache , value , strlen(value) )

  node->value.element.num_attrs ++;

  return (0);
}


/*
 * End of "$Id: mxml-attr.c 451 2014-01-04 21:50:06Z msweet $".
 */

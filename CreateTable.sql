--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

SET search_path = public, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: shelf; Type: TABLE; Schema: public; Owner: vince; Tablespace: 
--

CREATE TABLE shelf (
    bookid integer NOT NULL,
    title character varying(200),
    author character varying(200),
    publisher character varying(200),
    isbn character varying(200),
    genre character varying(200)
);


ALTER TABLE public.shelf OWNER TO vince;

--
-- Name: shelf_id_seq; Type: SEQUENCE; Schema: public; Owner: vince
--

CREATE SEQUENCE shelf_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.shelf_id_seq OWNER TO vince;

--
-- Name: shelf_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: vince
--

ALTER SEQUENCE shelf_id_seq OWNED BY shelf.bookid;


--
-- Name: bookid; Type: DEFAULT; Schema: public; Owner: vince
--

ALTER TABLE ONLY shelf ALTER COLUMN bookid SET DEFAULT nextval('shelf_id_seq'::regclass);


--
-- Data for Name: shelf; Type: TABLE DATA; Schema: public; Owner: vince
--

COPY shelf (bookid, title, author, publisher, isbn, genre) FROM stdin;
1	Book1	Vince	none	1	fiction
\.


--
-- Name: shelf_id_seq; Type: SEQUENCE SET; Schema: public; Owner: vince
--

SELECT pg_catalog.setval('shelf_id_seq', 7, true);


--
-- PostgreSQL database dump complete
--

